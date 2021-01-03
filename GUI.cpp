#include "PAG.h"
#include "RPC.h"
#include "SLI.h"
#include <ArduinoJson.h>
#include "GUI.h"
#include <HTTPClient.h>
#include <M5Core2.h>
#include <Preferences.h>
#include "WIF.h"
#include "BT4.h"
static Page *pages[50];
static uint8_t pageCount = 0;
static uint8_t currentPage = 0;
static bool t1 = false;
static bool t2 = false;
static bool t3 = false;
static uint32_t lastActive = 0;
static bool forceDownload = false;
static uint32_t sleepTimeout = SLEEP_TIMEOUT_LONG;
static String imgServer = "";
static uint32_t confVers = 0;
Preferences GUI__preferences;
void GUI_Init()
{
    String config = JsonRPC::init();
    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, config);

    // Test if parsing succeeds.
    if (error)
    {
        Serial.print(F("GUI ERR deserializeJson() of GUI Config failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println(F("GUI INF JSON Config loaded"));
    /**
     * Get some basic stuff as image server and timeout for sleep from config
     ***/
    imgServer = doc["imagesrv"].as<String>();
    confVers = doc["version"].as<uint32_t>();
    sleepTimeout = doc["sleepTimeout"].as<uint32_t>();
    /**
     * Try to get last config version to check if image files should be re-downloaded
     * **/
    GUI__preferences.begin("GUI", true);
    uint32_t lastVersion = GUI__preferences.getUInt("CONFVER", 0);
    GUI__preferences.end();
    if (confVers != lastVersion)
    {
        Serial.printf("GUI INF New config version found refreshing images old:%d, new:%d\r\n",lastVersion,confVers);
        forceDownload = true;
    }
    /**
     * Iterate through elements in config file
     ***/
    uint32_t elCount = doc["elements"].size();
    Serial.printf("M2I INF Number of Config Elemenets:%d\r\n", elCount);
    uint32_t x;
    for (x = 0; x < elCount; x++)
    {
        // Check type
        String type = doc["elements"][x]["type"].as<String>();
        Serial.printf("M2I Config Type: %s\r\n", type.c_str());
        // Typpe is slider page
        if (type == "SLI")
        {
            Serial.printf("GUI INF SLI Config: %s %s %s\r\n", doc["elements"][x]["image1"].as<String>().c_str(), doc["elements"][x]["id"].as<String>().c_str(), doc["elements"][x]["factor"].as<String>().c_str());
            pages[pageCount] = new SliderPage((JsonObject)(doc["elements"][x]));

            pageCount++;
        }
        // Type is 4 Button Page
        else if (type == "BT4")
        {
            Serial.printf("GUI INF B4T Config: %s \r\n", doc["elements"][x]["head"].as<String>().c_str());
            pages[pageCount] = new Button4Page((JsonObject)(doc["elements"][x]));
            pageCount++;
        }
    }
    /**
     * If new version was detected
     * Save the new version number in the nv memory.
     * As the page constructors are downloading the images the force download flag can be restetted
     ****/
    if (forceDownload)
    {

        forceDownload = false;
        GUI__preferences.begin("GUI", false);
        GUI__preferences.putUInt("CONFVER", confVers);
        GUI__preferences.end();
    }
    /***
     * Activate page 0 and render header
     **/
    pages[currentPage]->activate();
    GUI__header(pages[currentPage]->getHeader().c_str());
}

void GUI_Loop()
{
    /**
     * Checks if any button was pushed and changes the selected page (right/left) 
     * or sends the middle button to the page. Returns true if any touch was detected
     * used to reset the sleep timer. The sleeptimer fill overflow after 50 days
     ***/
    if (GUI__checkButtons())
    {
        lastActive = millis();
    }
    /**
     * Sleep timeout detected-->Goto light sleep and switch off not needed stuff.
     ***/
    if ((millis() - lastActive) > sleepTimeout)
    {
        Serial.println("GUI INF Timeout reached going to sleep");
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);
        //AxpSetBusPowerMode(1);
        // LCD - sleep mode
        M5.Lcd.sleep();
        // Periph power voltage preset (LCD_logic, SD card)
        M5.Axp.SetLDOVoltage(2, 0);
        // Touch - monitor mode
        GUI_TouchSetPowerMode(CST_POWER_MODE_MONITOR);
        // ESP32 - lower voltage
        M5.Axp.SetDCVoltage(0, 2700);

        M5.Axp.LightSleep(0);

        // ESP32 - restore voltage
        M5.Axp.SetDCVoltage(0, 3350);
        // Touch - active mode
        GUI_TouchSetPowerMode(CST_POWER_MODE_ACTIVE);
        // Periph power voltage preset (LCD_logic, SD card)
        M5.Axp.SetLDOVoltage(2, 3300);
       
        // Disable external 5V / enable internal 5V boost
        //AxpSetBusPowerMode(0);
        Serial.println("GUI INF Check WIFI Connection");
        /***
         * Re-Connect Wifi
         * Without reconnect it seems to be a bit random if
         * autoreconnect is working.
         ****/
        if(WIF_waitForReconnect())
        {
            Serial.println("GUI INF Wifi Connection established");
        }
        else
        {
            Serial.println("GUI ERR Wifi Connection failed");
        }
        /**
         * Finally switch the display on
         * and reset the wakeup timer
         * ****/
        M5.Lcd.wakeup();
        lastActive = millis();
    }

    pages[currentPage]->handleInput();
}
/**
 * Header Render function
 * */
void GUI__header(const char *string)
{
    M5.Lcd.setTextSize(1);
    M5.Lcd.fillRect(3, 4, 316, 20, BLACK);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);
}

/**
 * Write helper function for 1 Wire registers
 ***/
void GUI__WriteNByte(uint8_t addr, uint8_t reg, uint8_t num, uint8_t *data)
{
    Wire1.beginTransmission(addr);
    Wire1.write(reg);
    Wire1.write(data, num);
    Wire1.endTransmission();
}

/**
 * Function to set touch controller power mode
 * Options are  CST_POWER_MODE_ACTIVE
 *              CST_POWER_MODE_MONITOR
 ***/
void GUI_TouchSetPowerMode(uint8_t mode)
{
    uint8_t data;

    delay(200); // w/o delay hypernation doesn't work
    data = mode;
    GUI__WriteNByte(I2C_ADDR_TOUCH, 0xA5, 1, &data);
}

/**
 * Simple helper function to check if a touch is detected in a specific area
 * **/
bool GUI__isInArea(int xT, int yT, int x, int y, int sizeX, int sizeY)
{
    bool retVal = false;
    if (x != -1)
    {
        retVal = ((xT > 0) && (xT > x) && (xT < (x + sizeX)) && (yT > y) && (yT < (y + sizeY)));
    }
    return (retVal);
}
/**
 * Function to check the state of the 3 virtual touch buttons
 * returns true if any touch is detected.
 * **/ 
bool GUI__checkButtons()
{
    bool touchActive = false;
    TouchPoint_t pos = M5.Touch.getPressPoint();
    touchActive = (pos.x != -1);
    if (touchActive)
    {

        if (GUI__isInArea(pos.x, pos.y, T1_X, T1_Y, SIZE_X, SIZE_Y))
        {
            if (t1 == false)
            {
                t1 = true;
                pages[currentPage]->deActivate();

                if (currentPage == 0)
                {
                    currentPage = pageCount - 1;
                }
                else
                {
                    currentPage--;
                }
                Serial.printf("GUI INF T1 Pos x %d, Pos y %d , Page: %d\r\n", pos.x, pos.y, currentPage);
                pages[currentPage]->activate();
                GUI__header(pages[currentPage]->getHeader().c_str());
            }
        }
        else
        {
            if (t1 == true)
            {
                t1 = false;
            }
        }

        if (GUI__isInArea(pos.x, pos.y, T2_X, T2_Y, SIZE_X, SIZE_Y))
        {
            if (t2 == false)
            {
                t2 = true;
                pages[currentPage]->middleButtonPushed();
            }
        }
        else
        {
            if (t2 == true)
            {
                t2 = false;
            }
        }

        if (GUI__isInArea(pos.x, pos.y, T3_X, T3_Y, SIZE_X, SIZE_Y))
        {
            if (t3 == false)
            {
                t3 = true;

                pages[currentPage]->deActivate();
                currentPage++;
                if (currentPage >= pageCount)
                {
                    currentPage = 0;
                }
                Serial.printf("GUI INF T3 Pos x %d, Pos y %d , Page: %d\r\n", pos.x, pos.y, currentPage);
                pages[currentPage]->activate();
                GUI__header(pages[currentPage]->getHeader().c_str());
            }
        }
        else
        {
            if (t3 == true)
            {
                t3 = false;
            }
        }
    }
    else
    {
        t1 = false;
        t2 = false;
        t3 = false;
    }

    return (touchActive);
}

/**
 * Function checks if image is available and updtodate
 * if not it will download the image file from imgServer
 ***/
bool GUI_CheckImage(String path)
{
    bool retVal = true;
    if ((!SD.exists(path)) || (forceDownload))
    {
        if (SD.exists(path))
        {
            SD.remove(path);
        }
        M5.Lcd.print("Downloading Image\r\n");
        Serial.printf("GUI INF File: %s not found, downloading from %s", path.c_str(), imgServer.c_str());
        File file = SD.open(path, "a");
        HTTPClient http;

        http.begin(imgServer + path); //Specify the URL and certificate
        int httpCode = http.GET();    //Make the request

        if (httpCode == HTTP_CODE_OK)
        { //Check for the returning code
            http.writeToStream(&file);
        }
        else
        {
            retVal = false;
            Serial.printf("GUI ERR File: %s failed to download", path.c_str());
            M5.Lcd.print("Downloading Image failed\r\n");
        }

        file.close();
        Serial.printf("GUI INF File: %s downloaded and stored", path.c_str());
        http.end(); //Free the resources
    }
}
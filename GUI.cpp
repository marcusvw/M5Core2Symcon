#include "PAG.h"
#include "RPC.h"
#include "SLI.h"
#include <ArduinoJson.h>
#include "GUI.h"
static Page *pages[10];
static uint8_t pageCount = 0;
static uint8_t currentPage = 0;
static bool t1 = false;
static bool t2 = false;
static bool t3 = false;
void GUI_Init()
{
    String config = JsonRPC::init();
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, config);

    // Test if parsing succeeds.
    if (error)
    {
        Serial.print(F("GUI ERR deserializeJson() of GUI Config failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println(F("GUI INF JSON Config loaded"));
    uint32_t elCount = doc["elements"].size();
    Serial.printf("M2I INF Number of Config Elemenets:%d\r\n", elCount);
    uint32_t x;
    for (x = 0; x < elCount; x++)
    {
        String type = doc["elements"][x]["type"].as<String>();
        Serial.printf("M2I Config Type: %s\r\n", type.c_str());
        if (type = "SLI")
        {
            Serial.printf("M2I SLI Config: %s %s %s\r\n", doc["elements"][x]["image"].as<String>().c_str(), doc["elements"][x]["id"].as<String>().c_str(), doc["elements"][x]["factor"].as<String>().c_str());
            pages[pageCount] = new SliderPage(doc["elements"][x]["image"].as<String>(), doc["elements"][x]["id"].as<String>(), doc["elements"][x]["factor"].as<float>());
            pageCount++;
        }
        pages[currentPage]->activate();
    }
}

void GUI_Loop()
{
    GUI__checkButtons();
    pages[currentPage]->handleInput();
}

bool GUI__isInArea(int xT, int yT, int x, int y, int sizeX, int sizeY)
{
    bool retVal = false;
    if (x != -1)
    {
        retVal = ((xT > 0) && (xT > x) && (xT < (x + sizeX)) && (yT > y) && (yT < (y + sizeY)));
    }
    return (retVal);
}

void GUI__checkButtons()
{
    TouchPoint_t pos = M5.Touch.getPressPoint();
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

            pages[currentPage]->activate();
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
            pages[currentPage]->activate();
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
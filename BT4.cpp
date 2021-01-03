#include "BT4.h"
#include <M5Core2.h>
#include <ArduinoJson.h>
#include "GUI.h"
Button4Page::Button4Page(JsonObject obj)
{
    for(uint32_t x=0;x<BT4_NUM_BUT;x++)
    {
        Serial.printf("BT4 INF Loading Button: %d\r\n", x);
        imageOn[x] = obj["imagesOn"][x].as<String>();
        imageOff[x] = obj["imagesOff"][x].as<String>();
        itemId[x] = obj["ids"][x].as<String>();
        varOnly[x] = obj["varOnly"][x].as<bool>();
        GUI_CheckImage(imageOn[x]);
        GUI_CheckImage(imageOff[x]);
    }
    header = obj["head"].as<String>();
    Serial.printf("BT4 INF Button page loaded\r\n");
}

void Button4Page::activate()
{
    active = true;
    M5.Lcd.fillScreen(BLACK);
    handleInput();
    draw();
}
void Button4Page::deActivate()
{
    active = false;
}

String Button4Page::getHeader()
{
    return (header);
}
void Button4Page::draw()
{
    uint32_t z = 0;

    for (uint32_t x; x < BT4_NUM_BUT; x++)
    {
        if (btState[x])
        {
            M5.Lcd.drawBmpFile(SD, imageOn[x].c_str(), IMG_POS_X[x], IMG_POS_Y[x]);
        }
        else
        {
            M5.Lcd.drawBmpFile(SD, imageOff[x].c_str(), IMG_POS_X[x], IMG_POS_Y[x]);
        }
    }
}

void Button4Page::redrawBt(uint8_t img)
{

    if (btState[img])
    {
        M5.Lcd.drawBmpFile(SD, imageOn[img].c_str(), IMG_POS_X[img], IMG_POS_Y[img]);
    }
    else
    {
        M5.Lcd.drawBmpFile(SD, imageOff[img].c_str(), IMG_POS_X[img], IMG_POS_Y[img]);
    }
}
void Button4Page::middleButtonPushed()
{
}
bool Button4Page::isInArea(int xT, int yT, int x, int y, int sizeX, int sizeY)
{
    bool retVal = ((xT > 0) && (xT > x) && (xT < (x + sizeX)) && (yT > y) && (yT < (y + sizeY)));
    return (retVal);
}
void Button4Page::handleInput()
{
    char cstr[16];
    if (active)
    {
        TouchPoint_t pos = M5.Touch.getPressPoint();
        if (pos.x != -1)
        {
            bool tTState;
            for (int x; x < BT4_NUM_BUT; x++)
            {
                tTState = isInArea(pos.x, pos.y, IMG_POS_X[x], IMG_POS_Y[x], IMG_HEIGHT, IMG_WIDTH);
                if (tTState != tLastState[x])
                {
                    tLastState[x]=tTState;
                    if (tTState)
                    {
                        btState[x] = !btState[x];
                        if(varOnly[x])
                        {
                            if(btState[x])
                            {
                                JsonRPC::execute_boolean("SetValue", itemId[x] + ", true");
                            }
                            else
                            {
                                JsonRPC::execute_boolean("SetValue", itemId[x] + ", false");
                            }

                        }
                        else
                        {
                            if(btState[x])
                            {
                                JsonRPC::execute_boolean("RequestAction", itemId[x] + ", true");
                            }
                            else
                            {
                                JsonRPC::execute_boolean("RequestAction", itemId[x] + ", false");
                            }
                            
                            
                        }
                        
                        
                        redrawBt(x);
                    }
                }
            }
             lastUpdate = millis();
        }
        else
        {
            tLastState[0]=tLastState[1]=tLastState[2]=tLastState[3]=false;
            if ((millis() - lastUpdate) > 500)
            {
                for(uint32_t x=0;x<BT4_NUM_BUT;x++)
                {
                    bool uVal=false;
                    uVal = JsonRPC::execute_boolean("GetValue", String(itemId[x]));
                    if (JsonRPC::checkStatus())
                    {
                        
                        if (btState[x] != uVal)
                        {
                            btState[x]=uVal;
                            Serial.printf("BT4 INF State of Bt %d changed\r\n", uVal);
                            redrawBt(x);
                        }
                    }
                }
                lastUpdate = millis();
            }
        }
    }
}

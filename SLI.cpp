#include "SLI.h"
#include <M5Core2.h>
#include <ArduinoJson.h>
SliderPage::SliderPage(String paramImage, String paramItemId, float paramFactor)
{
    image = paramImage;
    itemId = paramItemId;
    factor = paramFactor;
}

void SliderPage::activate()
{
    active = true;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawFastVLine(SLIDER_X, SLIDER_Y, SLIDER_HEIGHT, WHITE);
    M5.Lcd.drawFastVLine(SLIDER_X + SLIDER_WIDTH, SLIDER_Y, SLIDER_HEIGHT, WHITE);
    M5.Lcd.drawBmpFile(SD, image.c_str(), 10, 60);
}
void SliderPage::deActivate()
{
    active = false;
}
void SliderPage::draw()
{
    uint32_t z = 0;
    for (z = 0; z < state; z++)
    {
        M5.Lcd.drawFastHLine(SLIDER_X + 5, (SLIDER_Y + 200) - (z * 2), (SLIDER_WIDTH - 10), WHITE);
        M5.Lcd.drawFastHLine(SLIDER_X + 5, (SLIDER_Y + 200) - (z * 2 + 1), (SLIDER_WIDTH - 10), BLACK);
    }
    for (z = state; z < 100; z++)
    {
        M5.Lcd.drawFastHLine(SLIDER_X + 5, (SLIDER_Y + 200) - (z * 2), (SLIDER_WIDTH - 10), BLACK);
        M5.Lcd.drawFastHLine(SLIDER_X + 5, (SLIDER_Y + 200) - (z * 2 + 1), (SLIDER_WIDTH - 10), BLACK);
    }
}
void SliderPage::middleButtonPushed()
{
    JsonRPC::execute_boolean("RequestAction", itemId + ",0");
    state=0;
    draw();
}
void SliderPage::handleInput()
{
    char cstr[16];
    if (active)
    {

        int32_t pos = getSliderPos();
        if (pos >= 0)
        {
            state = pos;
            draw();
            sliderActive = true;
            if ((millis() - lastUpdate) > 500)
            {

                itoa(state, cstr, 10);
                int val = (int)(factor * (float)state);
                JsonRPC::execute_boolean("RequestAction", itemId + "," + val);
                sliderLastSendValue = state;
                lastUpdate = millis();
            }
        }
        else
        {
            /* code */
            if (sliderActive)
            {
                itoa(state, cstr, 10);
                int val = (int)(factor * (float)state);
                JsonRPC::execute_boolean("RequestAction", itemId + "," + val);
                sliderLastSendValue = -11;
                state = -11;
                sliderActive = false;
                lastUpdate = millis();
            }
            else
            {
                if ((millis() - lastUpdate) > 500)
                {
                    lastUpdate = millis();
                    uint32_t uVal = JsonRPC::execute_int("GetValue", String(itemId));
                    if (JsonRPC::checkStatus())
                    {
                        int32_t sVal = (int32_t)(((float)(uVal)) / factor);
                        Serial.println("SLI INF Slider Value changed by server");
                        state = sVal;
                        draw();
                    }
                }
            }
        }
    }
}

int32_t SliderPage::getSliderPos()
{
    int retVal = -1;
    TouchPoint_t pos = M5.Touch.getPressPoint();
    if ((pos.x > SLIDER_X) && (pos.x < SLIDER_X + SLIDER_WIDTH))
    {
        if ((pos.y > SLIDER_Y) && (pos.y < (SLIDER_Y + SLIDER_HEIGHT)))
        {
            retVal = (SLIDER_HEIGHT - (pos.y - SLIDER_Y)) / 2;
        }
    }
    return (retVal);
}

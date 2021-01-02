#include "SLI.h"
#include "MQT.h"
#include <M5Core2.h>
SliderPage::SliderPage(String paramImage, String paramInTopic, String paramOutTopic)
{
    image = paramImage;
    inTopic = paramInTopic;
    outTopic = paramOutTopic;
    MQT_Subscribe(paramInTopic.c_str());
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
void SliderPage::handleInput()
{
    char cstr[16];
    if (active)
    {

        int32_t pos = getSliderPos();
        if (pos >= 0)
        {
            draw();
            sliderActive=true;
            state = pos;
            int32_t dif = sliderLastSendValue - state;
            if ((dif > 10) || (dif < -10))
            {
                itoa(state, cstr, 10);
                MQT_publish(outTopic.c_str(), cstr);
                sliderLastSendValue = state;
            }
        }
        else
        {
            /* code */
            if(sliderActive)
            {
                itoa(state, cstr, 10);
                MQT_publish(outTopic.c_str(), cstr);
                sliderLastSendValue = -11;
                state=-11;  
                sliderActive=false; 
            }
            String msg = MQT_getPayload(inTopic);
            if (msg.length() != 0)
            {
                
                int32_t sVal = atoi(msg.c_str());
                if (sVal != state)
                {
                    Serial.println("SLI INF Slider Value changed by server");
                    state = sVal;
                    draw();
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

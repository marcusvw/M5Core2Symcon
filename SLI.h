#pragma once
#include "PAG.h"
#include <M5Core2.h>
class SliderPage : Page
{
private:
    String image;
    String inTopic;
    String outTopic;
    uint32_t state;
    uint32_t sliderLastSendValue;
    bool sliderActive=false;
    const uint16_t SLIDER_X = 160;
    const uint16_t SLIDER_HEIGHT = 200;
    const uint16_t SLIDER_Y = 40;
    const uint16_t SLIDER_WIDTH = 150;
    const uint16_t IMG_POS_X = 10;
    const uint16_t IMG_POS_Y = 60;
    int32_t getSliderPos();
public:
    SliderPage(String paramImage, String paramInTopic, String paramOutTopic);
    void activate();
    void deActivate();
    void draw();
    void handleInput();
};
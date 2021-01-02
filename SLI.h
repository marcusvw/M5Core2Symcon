#pragma once
#include "PAG.h"
#include <M5Core2.h>
#include "RPC.h"
class SliderPage : public Page
{
private:
    String image;
    String itemId;
    float factor;
    uint32_t state;
    uint32_t sliderLastSendValue;
    JsonRPC rpc;
    bool sliderActive=false;
    const uint16_t SLIDER_X = 160;
    const uint16_t SLIDER_HEIGHT = 200;
    const uint16_t SLIDER_Y = 40;
    const uint16_t SLIDER_WIDTH = 150;
    const uint16_t IMG_POS_X = 10;
    const uint16_t IMG_POS_Y = 60;
    uint32_t lastUpdate=0;
    int32_t getSliderPos();
public:
    SliderPage(String paramImage, String paramItemId, float paramFactor);
    void activate();
    void deActivate();
    void draw();
    void handleInput();
    void middleButtonPushed();
};
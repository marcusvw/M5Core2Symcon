#pragma once
#include "PAG.h"
#include <M5Core2.h>
#include "RPC.h"
#include <ArduinoJson.h>
/**
 * Slider Page implementation
 ***/
class SliderPage : public Page
{
private:
    String image1;
    String image2;
    String itemId;
    String header;
    float factor;
    uint32_t state=0;
    uint32_t sliderLastSendValue;
    JsonRPC rpc;
    bool sliderActive=false;
    const uint16_t SLIDER_X = 160;
    const uint16_t SLIDER_HEIGHT = 200;
    const uint16_t SLIDER_Y = 25;
    const uint16_t SLIDER_WIDTH = 150;
    const uint16_t IMG_POS1_X = 32;
    const uint16_t IMG_POS1_Y = 32;
    const uint16_t IMG_POS2_X = 32;
    const uint16_t IMG_POS2_Y = 128;
    uint32_t lastUpdate=0;
    int32_t getSliderPos();
public:
    SliderPage(JsonObject obj);
    void activate();
    void deActivate();
    void draw();
    void handleInput();
    void middleButtonPushed();
    String getHeader();
};
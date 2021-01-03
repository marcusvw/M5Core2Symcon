#pragma once
#include "PAG.h"
#include <M5Core2.h>
#include "RPC.h"
#include <ArduinoJson.h>
#define BT4_NUM_BUT 4
class Button4Page : public Page
{
private:
    String imageOn[BT4_NUM_BUT];
    String imageOff[BT4_NUM_BUT];
    String itemId[BT4_NUM_BUT];
    bool tLastState[BT4_NUM_BUT];
    bool btState[BT4_NUM_BUT];
    bool varOnly[BT4_NUM_BUT];
    String header;
    JsonRPC rpc;
    const uint16_t IMG_POS_X[BT4_NUM_BUT] = {60,60,163,163};
    const uint16_t IMG_POS_Y[BT4_NUM_BUT] = {32,135,32,135};
    const uint16_t IMG_WIDTH = 96;
    const uint16_t IMG_HEIGHT = 96;
    uint32_t lastUpdate=0;
    static bool isInArea(int xT, int yT,int x,int y, int sizeX, int sizeY);
    void redrawBt(uint8_t img);
public:
    Button4Page(JsonObject obj);
    void activate();
    void deActivate();
    void draw();
    void handleInput();
    void middleButtonPushed();
    String getHeader();
};
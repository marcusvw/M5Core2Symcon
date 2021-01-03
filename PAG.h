#pragma once
#include <M5Core2.h>
/**
 * Abstract Class for GUI Page
 * Areas for M5Core2 Size is 320x215
 * **/
class Page
{
    protected:
    bool active=false;
    public:
    virtual void activate();
    virtual void deActivate();
    virtual void draw();
    virtual void handleInput();
    virtual void middleButtonPushed();
    virtual String getHeader();
         
};
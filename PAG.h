#pragma once
#include <M5Core2.h>
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
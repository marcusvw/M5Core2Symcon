class Page
{
    protected:
    bool active=false;
    public:
    virtual void activate();
    virtual void deActivate();
    virtual void draw();
    virtual void handleInput();     
};
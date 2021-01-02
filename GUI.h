#define OFFSET 5
#define SIZE_X 100
#define SIZE_Y 40 
#define T1_X (OFFSET)
#define T1_Y (240)
#define T2_X (T1_X+OFFSET+SIZE_X)
#define T2_Y (240)
#define T3_X (T2_X+OFFSET+SIZE_X)
#define T3_Y (240)
#define SLEEP_TIMEOUT_LONG 10000 /*ms*/
#define SLEEP_TIMEOUT_SHORT 500 /*ms*/
#define LIGHT_SLEEP_TIME 500000 /*us*/
bool GUI__isInArea(int xT, int yT,int x,int y, int sizeX, int sizeY);
void GUI_Init();
void GUI_Loop();
bool GUI__checkButtons();
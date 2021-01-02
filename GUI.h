#define OFFSET 5
#define SIZE_X 100
#define SIZE_Y 40 
#define T1_X (OFFSET)
#define T1_Y (240)
#define T2_X (T1_X+OFFSET+SIZE_X)
#define T2_Y (240)
#define T3_X (T2_X+OFFSET+SIZE_X)
#define T3_Y (240)
bool GUI__isInArea(int xT, int yT,int x,int y, int sizeX, int sizeY);
void GUI_Init();
void GUI_Loop();
void GUI__checkButtons();
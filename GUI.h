#define OFFSET 5
#define SIZE_X 100
#define SIZE_Y 50 
#define T1_X (OFFSET)
#define T1_Y (240)
#define T2_X (T1_X+OFFSET+SIZE_X)
#define T2_Y (240)
#define T3_X (T2_X+OFFSET+SIZE_X)
#define T3_Y (240)
#define SLEEP_TIMEOUT_LONG 10000 /*ms*/
#define I2C_ADDR_AXP 0x34
#define I2C_ADDR_TOUCH 0x38
#define CST_POWER_MODE_ACTIVE (0x00)
#define CST_POWER_MODE_MONITOR (0x01)
#define CST_POWER_MODE_HIBERNATE (0x03) // deep sleep
bool GUI__isInArea(int xT, int yT,int x,int y, int sizeX, int sizeY);
void GUI_Init();
void GUI_Loop();
bool GUI__checkButtons();
void GUI_TouchSetPowerMode(uint8_t mode);
void GUI__header(const char *string);
bool GUI_CheckImage(String path);
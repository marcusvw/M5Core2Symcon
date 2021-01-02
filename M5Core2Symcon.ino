#include <M5Core2.h>
#include "WIF.h"
#include <Preferences.h>
#include "SLI.h"
#include <ArduinoJson.h>
#include "RPC.h"
#include "GUI.h"
Preferences preferences;
SliderPage *sp[10];
uint32_t pages=0;
void setup()
{

  /**Initialize M5Stack stuff*/
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.print("Booting\r\n");
  Serial.setTimeout(2000);
  Serial.flush();
  Serial.println("Reset?(y)");
  String res = Serial.readString();
  res.trim();
  if (res == "y")
  {
    Serial.println("Reset Config");
    WIF_resetConfig();
    JsonRPC::resetConfig();
  }

  /**Init Serial**/
  WIF_init();
  GUI_Init();
  delay(100);
}

void loop()
{
  GUI_Loop();
  
}

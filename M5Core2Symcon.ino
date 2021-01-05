#include <M5Core2.h>
#include "src/wif/WIF.h"
#include <Preferences.h>
#include "src/gui/GUI.h"
#include <ArduinoJson.h>
#include "src/rpc/RPC.h"
Preferences preferences;
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
  
  /** Reset on request the config of the modules below**/
  Serial.println("Reset?(y)");
  String res = Serial.readString();
  res.trim();
  if (res == "y")
  {
    Serial.println("Reset Config");
    WIF_resetConfig();
    JsonRPC::resetConfig();
  }

  /**Init WIFI**/
  WIF_init();
  /**Init GUI**/
  GUI_Init();
  delay(100);
}

void loop()
{
  /** GUI Loop does all the job in regards to Pages/Touch etc.**/
  GUI_Loop();
  
}

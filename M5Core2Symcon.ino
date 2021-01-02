#include <M5Core2.h>
#include "WIF.h"
#include "MQT.h"
#include <Preferences.h>
#include "SLI.h"
#include <ArduinoJson.h>

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
    MQT_resetConfig();
    WIF_resetConfig();
  }

  /**Init Serial**/
  WIF_init();
  MQT_init();
  Serial.println("M2I INF Booting finished Finished");
  M5.Lcd.print("waiting for config\r\n");
  while(1)
  {
    MQT_loop();
    if(MQT_isConfigured())
    {  
      break;
    }
  }
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, MQT_getConfig());

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("M2I ERR deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  Serial.println(F("M2I INF JSON Config loaded"));
  uint32_t elCount=doc["elements"].size();
  Serial.printf("M2I INF Number of Config Elemenets:%d\r\n",elCount);
  uint32_t x;
  for(x=0;x<elCount;x++)
  {
    String type = doc["elements"][x]["type"].as<String>();
    Serial.printf("M2I Config Type: %s\r\n",type.c_str());
    if(type="SLI")
    {
      Serial.printf("M2I SLI Config: %s %s %s\r\n",doc["elements"][x]["image"].as<String>().c_str(), doc["elements"][x]["int"].as<String>().c_str(), doc["elements"][x]["out"].as<String>().c_str());
       sp[pages]=new SliderPage(doc["elements"][x]["image"].as<String>(), doc["elements"][x]["int"].as<String>(), doc["elements"][x]["out"].as<String>());
       pages++;
    }
  }
  
  sp[0]->activate();
  delay(100);
}

void loop()
{
  sp[0]->handleInput();
  MQT_loop();
}

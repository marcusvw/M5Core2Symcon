#include <Preferences.h>
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include "WIF.h"
#include <M5Core2.h>
#include <Wire.h>

/***********************************************
 * Global Var Area
 * ************************************************/

static char WIF__wifiSSID[WIF__SSID_BUFFER_SIZE];
static char WIF__wifiPSK[WIF__PSK_BUFFER_SIZE];
Preferences WIF__preferences;

void WIF_resetConfig()
{
    WIF__preferences.begin(WIF_PRE_DIR, false);
    WIF__preferences.clear();
    WIF__preferences.end();
}
void WIF_init()
{
    bool isConfigured;
    Serial.println(F("WIF INF Reading Config"));
    /*Open Prereferences memory in read/write mode*/
    WIF__preferences.begin(WIF_PRE_DIR, false);
    isConfigured = WIF__preferences.getBool(WIF_PRE_KEY_IS_CONFIGURED, false);
    if (!isConfigured)
    {
        Serial.println("WIF INF No valid config found");
        M5.Lcd.print("Config failed, check serial\r\n");
        WIF__enterConfig();
    }
    else
    {
        WIF__preferences.getString(WIF_PRE_KEY_PSK, WIF__wifiPSK, WIF__PSK_BUFFER_SIZE);
        WIF__preferences.getString(WIF_PRE_KEY_SSID, WIF__wifiSSID, WIF__SSID_BUFFER_SIZE);
        if(WIF__connectWifi())
        {
            Serial.println(F("WIF INF WIFI Succesfully Connected"));
            Serial.print("WIF INF M5 IP: ");
            Serial.println(WiFi.localIP());
            M5.Lcd.printf("WIFI IP:%s\r\n",WiFi.localIP().toString().c_str());
        }
        else
        {
            Serial.println(F("WIF INF WIFI Not connected"));
            M5.Lcd.print("WIFI not connected");
        }
        
    }
}
void WIF__enterConfig()
{
    Serial.setTimeout(200000);
    WIF__scanWifiNetworks();
    WIF__getPSK();
    WIF__preferences.putString(WIF_PRE_KEY_PSK, WIF__wifiPSK);
    WIF__preferences.putString(WIF_PRE_KEY_SSID, WIF__wifiSSID);
    WIF__preferences.putBool(WIF_PRE_KEY_IS_CONFIGURED, true);
}

void WIF__getPSK()
{
    while (1)
    {
        Serial.println(F("WIF 001 Enter PSK/Wifi Password:"));
        String psk = Serial.readStringUntil('\n');
        psk.trim();
        psk.toCharArray(WIF__wifiPSK, WIF__PSK_BUFFER_SIZE);
        Serial.println(psk);
        if (WIF__connectWifi())
        {
            Serial.println(F("WIF COK WIFI Succesfully Connected"));
            Serial.print("M5 IP: ");
            Serial.println(WiFi.localIP());
            break;
        }
        else
        {
            Serial.println(F("PSK Wrong, try again"));
        }
    }
}

bool WIF__connectWifi()
{
    bool retVal = true;
    uint64_t startTime = millis();
    WiFi.begin(WIF__wifiSSID, WIF__wifiPSK);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        if (millis() - startTime > WIF__WIFI_TIMEOUT)
        {
            WiFi.disconnect(true, true);
            retVal = false;
            break;
        }
    }
    return (retVal);
}

void WIF__scanWifiNetworks()
{
    String ssidsArray[50];
    String networkString;
    int32_t clientWifiSsidId = 0;
    Serial.println("Scanning for Wifi Networks");
    WiFi.mode(WIFI_STA);
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    if (n == 0)
    {
        Serial.println("WIF INF no networks found");
    }
    else
    {
        Serial.print("WIF INF ");
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            ssidsArray[i + 1] = WiFi.SSID(i);
            networkString = i + 1;
            networkString = networkString + ": " + WiFi.SSID(i) + " (Strength:" + WiFi.RSSI(i) + ")";
            Serial.println(networkString);
        }
        while (1)
        {
            Serial.flush();
            String selection = Serial.readStringUntil('\n');
            selection.trim();
            clientWifiSsidId = selection.toInt();
            Serial.println(selection);
            if (clientWifiSsidId > n)
            {
                Serial.println(F("WIF INF Incorrect selection"));
            }
            else
            {
                ssidsArray[clientWifiSsidId].toCharArray(WIF__wifiSSID, WIF__SSID_BUFFER_SIZE);
                Serial.println(F("WIF INF Network Selected"));
                Serial.println("WIF INF "+ssidsArray[clientWifiSsidId]);
                break;
            }
        }
    }
}

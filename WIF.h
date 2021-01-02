#pragma once
void WIF_init();
void WIF__getPSK();
void WIF__enterConfig();
bool WIF__connectWifi();
void WIF__scanWifiNetworks();
void WIF_resetConfig();
/***********************************************
 * Define Area
 * ************************************************/
#define WIF__SSID_BUFFER_SIZE 64U
#define WIF__PSK_BUFFER_SIZE 64U
#define WIF_PRE_KEY_SSID "SSID"
#define WIF_PRE_KEY_PSK "PSK"
#define WIF_PRE_KEY_MQTT_SERVER "MQTT_SERVER"
#define WIF_PRE_KEY_MQTT_PORT "MQTT_PORT"
#define WIF_PRE_KEY_IS_CONFIGURED "IS_CONFIGURED"
#define WIF_PRE_DIR "WIF"
#define WIF__WIFI_TIMEOUT 10000U
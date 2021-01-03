#pragma once
#include <WiFi.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <M5Core2.h>
#define RPC__HOST_LENGTH 128
#define RPC__USER_LENGTH 64
#define RPC__PWD_LENGTH 64
#define RPC__PRE_DIR "JSONRPC"
#define RPC__PRE_KEY_IS_CONFIGURED "IS_CONFIG"
#define RPC__PRE_KEY_HOST "HOST"
#define RPC__PRE_KEY_USER "USER"
#define RPC__PRE_KEY_PWD "PWD"
#define RPC__PRE_KEY_CONFIG_ID "CID"
/**
 * JsonRPC Class for connectio to Symcon
 ****/
class JsonRPC
{
    public:
    static String execute_string(String method, String parameters);
    static uint32_t execute_int(String method, String parameters);
    static double execute_float(String method, String parameters);
    static bool execute_boolean(String method, String parameters);
    static bool checkStatus();
    static void resetConfig();
    static String init();
    private:
    static String enterConfig();
    static bool execute(String method, String parameters);
    static String jsonPayload;
    static HTTPClient http;
    static WiFiClient wifiClient;
    static Preferences preferences;
    static char host[RPC__HOST_LENGTH];
    static char user[RPC__USER_LENGTH];
    static char password[RPC__PWD_LENGTH];
    static uint32_t cId;
    static bool isConfigured;
    static bool lastStatus;
};

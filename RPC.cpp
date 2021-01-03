#include <WiFi.h>
#include <HTTPClient.h>
#include "RPC.h"
#include <ArduinoJson.h>


/****
 * Init/instatiations of static members
 * Thw whole class is static
 *****/
HTTPClient JsonRPC::http;
Preferences JsonRPC::preferences;
String JsonRPC::jsonPayload="";
bool JsonRPC::lastStatus=true;
char JsonRPC::host[]="";
char JsonRPC::password[]="";
char JsonRPC::user[]="";
uint32_t JsonRPC::cId;
bool JsonRPC::isConfigured;


/***
 * Init function check if config is available, if not config via
 * serial is requested
 ****/
String JsonRPC::init()
{
    String response;
    /***
     * Open preferences for RPC module
     ***/
    preferences.begin(RPC__PRE_DIR, false);
    /**
     * Check if config is available
     ****/
    isConfigured = preferences.getBool(RPC__PRE_KEY_IS_CONFIGURED, false);
    /***
     * In case of none valid/reset configuration
     * call enterConfig function
     *****/
    if (!isConfigured)
    {
        Serial.println("RPC INF No valid config found");
        M5.Lcd.print("Config failed, check serial\r\n");
        /**
         * After sucessfull config the GUI Json is request from server and returned
         ***/
        response=enterConfig();
    }
    else
    {
        /**
         * valid config found, read the config
         * ***/
        preferences.getString(RPC__PRE_KEY_HOST, host, RPC__HOST_LENGTH);
        preferences.getString(RPC__PRE_KEY_USER, user, RPC__USER_LENGTH);
        preferences.getString(RPC__PRE_KEY_PWD, password, RPC__PWD_LENGTH);
        cId = preferences.getUInt(RPC__PRE_KEY_CONFIG_ID, 0);
        /***
         * Try to read gui config from Symcon Server
         ****/
        response = execute_string("GetValue", String(cId));
        if(lastStatus)
        {
            Serial.println("RPC INF Config:"+response);
        }
        else
        {
            Serial.println("RPC ERR Config Download failed");
        }
    }
    preferences.end();
    /**
     *return String with GUI config json
     ***/
    return(response);
}

/**
 * Function to configure:
 * - Symcon Server
 * - User
 * - Password
 * - ID of variable where the JSON GUI Config is stored
 ***/
String JsonRPC::enterConfig()
{
    String response;
    /** Set timeout too a long time**/
    Serial.setTimeout(200000);
    while (1)
    {
        /** Request parameter from user via Serial Interface, no echo*/
        Serial.println(F("RPC 001 Enter Symcon Host(full URL to API):"));
        String hostStr = Serial.readStringUntil('\n');
        hostStr.trim();
        hostStr.toCharArray(host, RPC__HOST_LENGTH);
        Serial.println(host);

        Serial.println(F("RPC 002 Enter User:"));
        String userStr = Serial.readStringUntil('\n');
        userStr.trim();
        userStr.toCharArray(user, RPC__USER_LENGTH);
        Serial.println(user);

        Serial.println(F("RPC 003 Enter Password:"));
        String passwordStr = Serial.readStringUntil('\n');
        passwordStr.trim();
        passwordStr.toCharArray(password, RPC__PWD_LENGTH);
        Serial.println(password);

        Serial.println(F("RPC 004 Enter Config Variable ID:"));
        String cidStr = Serial.readStringUntil('\n');
        cidStr.trim();
        cId=cidStr.toInt();
        Serial.println(cId);

        /**
         * Test parameters and request config
         **/
        response = execute_string("GetValue", String(cId));
        /**
         * Last request/Get config was succesfull
         * Store parameters in nv memory
         ***/
        if(lastStatus)
        {
            Serial.println("RPC INF Config:"+response);
            preferences.putString(RPC__PRE_KEY_HOST, host);
            preferences.putString(RPC__PRE_KEY_USER, user);
            preferences.putString(RPC__PRE_KEY_PWD, password);
            preferences.putUInt(RPC__PRE_KEY_CONFIG_ID, cId);
            preferences.putBool(RPC__PRE_KEY_IS_CONFIGURED, true);
            break;
        }
        else
        {
            Serial.println("RPC ERR Config Download failed");
        }
        
    }
    /** return Json config string**/
    return(response);
}
/**
 * Reset Config, enable reconfig
 ***/
void JsonRPC::resetConfig()
{
    preferences.begin(RPC__PRE_DIR, false);
    preferences.clear();
}

/***
 * Base function to send request to symcon rpc api
 ***/
bool JsonRPC::execute(String method, String parameters)
{
    bool retVal = true;
    http.begin(host);
    http.setAuthorization(user, password);
    http.addHeader("Content-Type", "application/json", false, true);
    String req = "{\"jsonrpc\": \"2.0\", \"id\": \"0\", \"method\": \"" + method + "\", \"params\": [" + parameters + "]}";
    Serial.println("RPC INF Request: " + req);
    int postRes = http.POST(req);
    if (postRes == HTTP_CODE_OK)
    {
        jsonPayload = http.getString();
        Serial.println("RPC INF:" + jsonPayload);
    }
    else
    {
        Serial.printf("RPC INF failed %d:", retVal);
        retVal = false;
    }

    http.end();
    lastStatus=retVal;
    return (retVal);
}
/**
 * Function returns the status of last request
 ****/
bool JsonRPC::checkStatus()
{
    return(lastStatus);
}

/**
 * Execute RPC Call with return value String (also JSON)
 **/
String JsonRPC::execute_string(String method, String parameters)
{
    String retVal = "";
    if (execute(method, parameters))
    {

        if (jsonPayload.length() != 0)
        {
            Serial.println("RPC INF JSON Interpretation");
             DynamicJsonDocument doc(20000);
            DeserializationError error = deserializeJson(doc, jsonPayload);

            if (error)
            {
                Serial.print(F("SLI ERR deserializeJson() failed: "));
                Serial.println(error.c_str());
                lastStatus=false;
            }
            else
            {
                Serial.println("SLI INF JSON Interpreted");
                Serial.println(doc["result"].as<String>());
                retVal = doc["result"].as<String>();
            }
        }
        else
        {
            lastStatus=false;
        }
        
    }

    return (retVal);
}
/**
 * Execute RPC Call with return value int
 **/
uint32_t JsonRPC::execute_int(String method, String parameters)
{
     uint32_t retVal = 0;
    if (execute(method, parameters))
    {

        if (jsonPayload.length() != 0)
        {
            Serial.println("RPC INF JSON Interpretation");
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, jsonPayload);

            if (error)
            {
                Serial.print(F("SLI ERR deserializeJson() failed: "));
                Serial.println(error.c_str());
                lastStatus=false;
            }
            else
            {
                Serial.println("SLI INF JSON Interpreted");
                Serial.println(doc["result"].as<String>());
                retVal = doc["result"];
            }
        }
        else
        {
            lastStatus=false;
        }
        
    }

    return (retVal);
}

/**
 * Execute RPC Call with return value float
 **/
double JsonRPC::execute_float(String method, String parameters)
{
     double retVal = 0.0;
    if (execute(method, parameters))
    {

        if (jsonPayload.length() != 0)
        {
            Serial.println("RPC INF JSON Interpretation");
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, jsonPayload);

            if (error)
            {
                Serial.print(F("SLI ERR deserializeJson() failed: "));
                Serial.println(error.c_str());
                lastStatus=false;
            }
            else
            {
                Serial.println("SLI INF JSON Interpreted");
                Serial.println(doc["result"].as<String>());
                retVal = doc["result"];
            }
        }
        else
        {
            lastStatus=false;
        }
        
    }

    return (retVal);
}
/**
 * Execute RPC Call with return value boolean
 ***/
bool JsonRPC::execute_boolean(String method, String parameters)
{
     bool retVal = false;
    if (execute(method, parameters))
    {

        if (jsonPayload.length() != 0)
        {
            Serial.println("RPC INF JSON Interpretation");
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, jsonPayload);

            if (error)
            {
                Serial.print(F("SLI ERR deserializeJson() failed: "));
                Serial.println(error.c_str());
                lastStatus=false;
            }
            else
            {
                Serial.println("SLI INF JSON Interpreted");
                Serial.println(doc["result"].as<String>());
                retVal = doc["result"];
            }
        }
        else
        {
            lastStatus=false;
        }
        
    }

    return (retVal);
}

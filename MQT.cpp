
#include <M5Core2.h>
#include <ETH.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Preferences.h>
#include <Dictionary.h>
#include "MQT.h"
WiFiClient MQT__wifiClient;
PubSubClient MQT__mqttClient(MQT__wifiClient);
char MQT__clientID[MQT__CLIENT_ID_BUFFER_SIZE];
char MQT__Server[MQT__SERVER_BUFFER_SIZE];
char MQT__ConfigTopic[MQT__CONFIG_TOPIC_BUFFER_SIZE];
uint16_t MQT__port;
Preferences MQT__preferences;
Dictionary MQT__TopicValueDir;
bool MQT__isConfigured = false;
char MQT__config[1024];

bool MQT_isConfigured()
{
  return(MQT__isConfigured);
}

char * MQT_getConfig()
{
    return(MQT__config);
}

void MQT_Subscribe(String topic)
{
    if (!MQT__TopicValueDir(topic))
    {
        MQT__mqttClient.subscribe(topic.c_str());
        MQT__TopicValueDir[topic] = "";
        Serial.printf("MQT INF New Topic subscribed: %s\r\n", topic.c_str());
    }
}

void MQT_callback(char *topic, byte *payload, unsigned int length)
{
    char strm[254];

    if (strcmp(MQT__ConfigTopic, topic) == 0)
    {
        Serial.println(F("MQT INF Config payload received"));
        memcpy(MQT__config, payload, length);
        MQT__config[length] = 0;
        MQT__isConfigured =true;
    }
    else
    {
        if (length > 254)
        {
            Serial.printf("MQT ERR Payload to big: %s\r\n", topic);
        }
        else
        {
            memcpy(strm, payload, length);
            strm[length] = 0;
            MQT__TopicValueDir(topic, strm);
            Serial.printf("MQT INF Message Reveived: %s , %s\r\n", topic, strm);
        }
    }
    
}

void MQT_resetConfig()
{
    MQT__preferences.begin(MQT_PRE_DIR, false);
    MQT__preferences.clear();
    MQT__preferences.end();
}

void MQT_publish(const char *topic, const char *payload)
{
    Serial.printf("MQT INF Message Send: %s , %s\r\n", topic, payload);
    MQT__mqttClient.publish(topic, payload);
}

void MQT__enterConfig()
{
    while (1)
    {
        Serial.println(F("MQT 001 Enter MQTT Server (IP or DNS Name):"));
        String server = Serial.readStringUntil('\n');
        server.trim();
        server.toCharArray(MQT__Server, MQT__SERVER_BUFFER_SIZE);
        Serial.println(server);
        Serial.println(F("MQT 002 Enter MQTT Server Port:"));
        String port = Serial.readStringUntil('\n');
        port.trim();
        Serial.println(port);
        MQT__port = port.toInt();
        Serial.println(F("MQT 003 Enter Client ID:"));
        String clientId = Serial.readStringUntil('\n');
        clientId.trim();
        Serial.println(clientId);
        clientId.toCharArray(MQT__clientID, MQT__CLIENT_ID_BUFFER_SIZE);
        Serial.println(F("MQT 004 Enter Config Topic:"));
        String configTopic = Serial.readStringUntil('\n');
        configTopic.trim();
        Serial.println(configTopic);
        configTopic.toCharArray(MQT__ConfigTopic, MQT__CONFIG_TOPIC_BUFFER_SIZE);
        if (MQT__reconnect())
        {
            Serial.println(F("MQT COK Sucessfully connected to MQTT Server"));
            break;
        }
        else
        {
            Serial.println(F("MQT CER Connection Failed"));
        }
    }

    MQT__preferences.putString(MQT_PRE_KEY_CLIENT_ID, MQT__clientID);
    MQT__preferences.putString(MQT_PRE_KEY_SERVER, MQT__Server);
    MQT__preferences.putString(MQT_PRE_KEY_CONFIG_TOPIC, MQT__ConfigTopic);
    MQT__preferences.putUShort(MQT_PRE_KEY_PORT, MQT__port);
    MQT__preferences.putBool(MQT_PRE_KEY_IS_CONFIGURED, true);
}

boolean MQT_init()
{
    bool isConfigured;
    Serial.setTimeout(20000);
    Serial.println(F("MQT INF Reading Config"));
    /*Open Prereferences memory in read/write mode*/
    MQT__preferences.begin(MQT_PRE_DIR, false);
    MQT__mqttClient.setCallback(MQT_callback);
    isConfigured = MQT__preferences.getBool(MQT_PRE_KEY_IS_CONFIGURED, false);
    if (!isConfigured)
    {
        Serial.println("MQT INF No valid config found");
        M5.Lcd.printf("MQTT Config missing check serial\r\n");
        MQT__enterConfig();
    }
    else
    {
        MQT__preferences.getString(MQT_PRE_KEY_CLIENT_ID, MQT__clientID, MQT__CLIENT_ID_BUFFER_SIZE);
        MQT__preferences.getString(MQT_PRE_KEY_SERVER, MQT__Server, MQT__SERVER_BUFFER_SIZE);
        MQT__port = MQT__preferences.getUShort(MQT_PRE_KEY_PORT, 1803);
        MQT__preferences.getString(MQT_PRE_KEY_CONFIG_TOPIC, MQT__ConfigTopic, MQT__CONFIG_TOPIC_BUFFER_SIZE);
        if (MQT__reconnect())
        {
            Serial.println(F("MQT INF MQTT Succesfull connected"));
        }
    }
}

boolean MQT__reconnect()
{
    boolean retVal = true;
    MQT__mqttClient.setServer(MQT__Server, MQT__port);
    // Loop until we're reconnected
    if (!MQT__mqttClient.connected())
    {
        Serial.println("MQT INF Attempting MQTT connection...");

        if (MQT__mqttClient.connect(MQT__clientID))
        {
            Serial.println("MQT INF connected");
            M5.Lcd.printf("MQTT Connection established\r\n");
            MQT__mqttClient.subscribe(MQT__ConfigTopic);
            uint32_t cnt = MQT__TopicValueDir.count();
            for (uint32_t i = 0; i < cnt; i++)
            {
                MQT__mqttClient.subscribe(MQT__TopicValueDir.key(i).c_str());
            }
        }
        else
        {
            Serial.print("MQT INF Connection failed, rc=");
            Serial.println(MQT__mqttClient.state());
            Serial.printf("MQT INF Server:%s:%d\r\n", MQT__Server, MQT__port);
            M5.Lcd.printf("MQTT Connection failed\r\n");
            retVal = false;
        }
    }
    return (retVal);
}

String MQT_getPayload(String topic)
{
    return (MQT__TopicValueDir[topic]);
}

void MQT_loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (MQT__reconnect())
        {
            MQT__mqttClient.loop();
        }
    }
}
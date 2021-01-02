#pragma once
#include <M5Core2.h>
#define MQT__SERVER_BUFFER_SIZE 64
#define MQT__CLIENT_ID_BUFFER_SIZE 64
#define MQT__CONFIG_TOPIC_BUFFER_SIZE 64
#define MQT_PRE_DIR "MQT"
#define MQT_PRE_KEY_IS_CONFIGURED "IS_CONFIGURED"
#define MQT_PRE_KEY_SERVER "SERVER"
#define MQT_PRE_KEY_CLIENT_ID "CLIENT_ID"
#define MQT_PRE_KEY_PORT "PORT"
#define MQT_PRE_KEY_CONFIG_TOPIC "CONFIG_TOPIC"

void MQT__enterConfig();
boolean MQT_init();
boolean MQT__reconnect();
void MQT_resetConfig();
void MQT_loop();
void MQT_publish(const char * topic,const char * payload);
String MQT_getPayload(String topic);
void MQT_Subscribe(String topic);
bool MQT_isConfigured();
char * MQT_getConfig();

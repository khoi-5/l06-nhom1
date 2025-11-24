#ifndef __COREIOT_H__
#define __COREIOT_H__

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "global.h"

// ===== LED PIN =====
#define LED_PIN 48    // đổi theo board của bạn (ESP32-S3 có thể khác)

// ===== Các biến sensor (được định nghĩa ở file khác) =====
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;

// ===== Semaphore báo có Internet (được tạo ở chỗ WiFi) =====
extern SemaphoreHandle_t xBinarySemaphoreInternet;

// ===== Cấu hình Adafruit IO (định nghĩa ở coreiot_common.cpp) =====
extern const char* AIO_USERNAME;
extern const char* AIO_KEY;

// tên feed
extern const char* FEED_TEMP;       // "temperature"
extern const char* FEED_HUMID;      // "humidity"
extern const char* FEED_LIGHT;      // "light"
extern const char* FEED_LED;        // "led"
extern const char* FEED_LIGHT_CTRL; // "light-control"
extern const char* FEED_TEMP_CTRL;  // "temperature-control"
extern const char* FEED_HUMID_CTRL; // "humidity-control"
extern const char* FEED_HTL;
extern const char* FEED_HTL_CTRL;
extern const char* FEED_LED_NEO;
extern const char* FEED_WIFI;
extern const char* FEED_CYCLE;

// ===== MQTT client dùng chung =====
extern WiFiClient espClient;
extern PubSubClient client;

// ===== Topic đầy đủ <username>/feeds/<feed> =====
extern String baseTopic;
extern String topicTemp;
extern String topicHumid;
extern String topicLight;
extern String topicLed;
extern String topicLightCtrl;
extern String topicTempCtrl;
extern String topicHumidCtrl;
extern String topicHTL;
extern String topicHTLCtrl;
extern String topicLedNeo;
extern String topicWifiCfg;
extern String topicCycle;

// ===== Trạng thái control dùng chung =====
extern bool g_lightEnable;
extern bool g_tempEnable;
extern bool g_humidEnable;


extern bool isAPMode;
extern bool isWifiConnected;
extern bool connecting;
extern unsigned long connect_start_ms;

// ===== Hàm dùng chung =====
void setup_coreiot();   // chờ Internet + setServer + setCallback
void reconnect();       // reconnect MQTT, subscribe các control feed

// callback xử lý message MQTT (được định nghĩa ở coreiot_subscribe.cpp)
void mqttCallback(char* topic, byte* payload, unsigned int length);

// Task chính gửi/poll MQTT (được định nghĩa ở coreiot_publish.cpp)
void coreiot_task(void *pvParameters);

#endif

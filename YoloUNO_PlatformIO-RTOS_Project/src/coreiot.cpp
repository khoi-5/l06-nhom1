

#include <WiFi.h>
#include <PubSubClient.h>
#include "coreiot.h"
#include "aio.h"   // MY_AIO_USERNAME, MY_AIO_KEY, AIO_SERVER, 

// ================== CẤU HÌNH ADAFRUIT IO ==================
const char* AIO_USERNAME = MY_AIO_USERNAME;
const char* AIO_KEY      = MY_AIO_KEY;

// ================== TÊN FEED ==================
const char* FEED_TEMP       = "dadn.temp";
const char* FEED_HUMID      = "dadn.humid";
const char* FEED_LIGHT      = "dadn.light";
const char* FEED_MOISTURE   = "dadn.moisture";
const char* FEED_DATA       = "dadn.data";

const char* FEED_LED        = "dadn.led";
const char* FEED_LIGHT_CTRL = "light-control";
const char* FEED_TEMP_CTRL  = "temperature-control";
const char* FEED_HUMID_CTRL = "humidity-control";
const char* FEED_HTL        = "humidity-temperature-light";
const char* FEED_HTL_CTRL   = "humidity_temperature_light_control";
const char* FEED_LED_NEO    = "led-neo";
const char* FEED_WIFI       = "wifi-id-password";
const char* FEED_CYCLE      = "chu-ky";

const char* FEED_FAN        = "dadn.fan";
const char* FEED_MAYBOM     = "dadn.maybom";
const char* FEED_PHANTRAM = "dadn.phantram";

// ================== CLIENT ==================
WiFiClient espClient;
PubSubClient client(espClient);

// ================== TOPIC ĐẦY ĐỦ ==================
String baseTopic      = String(AIO_USERNAME) + "/feeds/";
String topicTemp      = baseTopic + FEED_TEMP;
String topicHumid     = baseTopic + FEED_HUMID;
String topicLight     = baseTopic + FEED_LIGHT;
String topicMoisture  = baseTopic + FEED_MOISTURE;
String topicData      = baseTopic + FEED_DATA;

String topicLed       = baseTopic + FEED_LED;
String topicLightCtrl = baseTopic + FEED_LIGHT_CTRL;
String topicTempCtrl  = baseTopic + FEED_TEMP_CTRL;
String topicHumidCtrl = baseTopic + FEED_HUMID_CTRL;
String topicHTL       = baseTopic + FEED_HTL;
String topicHTLCtrl   = baseTopic + FEED_HTL_CTRL;
String topicLedNeo    = baseTopic + FEED_LED_NEO;
String topicWifiCfg   = baseTopic + FEED_WIFI;
String topicCycle     = baseTopic + FEED_CYCLE;

String topicFan       = baseTopic + FEED_FAN;
String topicMaybom    = baseTopic + FEED_MAYBOM;
String topicPHANTRAM  = baseTopic + FEED_PHANTRAM;

// ================== TRẠNG THÁI CONTROL ==================
bool g_lightEnable    = true;
bool g_tempEnable     = true;
bool g_humidEnable    = true;
bool g_moistureEnable = true;
bool g_data           = true;

// ================== BIẾN NGOÀI ==================
extern SemaphoreHandle_t xBinarySemaphoreInternet;
extern void mqttCallback(char* topic, byte* payload, unsigned int length);

// ==================RECONNECT ==================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting connection to Adafruit IO... ");

    String clientId = "ESP32Client-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), AIO_USERNAME, AIO_KEY)) {
      Serial.println("connected!");

      client.subscribe(topicLed.c_str());
      client.subscribe(topicFan.c_str());
      client.subscribe(topicMaybom.c_str());
      // client.subscribe(topicPHANTRAM.c_str());

      Serial.print("Subscribed: "); Serial.println(topicLed);
      Serial.print("Subscribed: "); Serial.println(topicFan);
      Serial.print("Subscribed: "); Serial.println(topicMaybom);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5 seconds");
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

// ================== SETUP ==================
void setup_coreiot() {
  while (xSemaphoreTake(xBinarySemaphoreInternet, pdMS_TO_TICKS(500)) == pdFALSE) {
    Serial.print(".");
  }
  Serial.println(" Connected!");

  client.setServer(AIO_SERVER, mqttPort);
  client.setCallback(mqttCallback);
}
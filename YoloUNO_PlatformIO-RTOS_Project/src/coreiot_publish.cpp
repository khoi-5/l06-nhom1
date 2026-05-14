#include "coreiot.h"
#include <ArduinoJson.h>
#include <math.h>

// ================== BIẾN SENSOR TOÀN CỤC ==================
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;
extern float glob_moisture;

// ở chỗ khác đã có
extern int chu_ky;

// ================== TASK CHÍNH GỬI/POLL ==================
void coreiot_task(void *pvParameters) {
  setup_coreiot();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  TickType_t lastPublish = xTaskGetTickCount();

  while (1) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    TickType_t now = xTaskGetTickCount();

    int chuKySafe = chu_ky;
    if (chuKySafe < 1) chuKySafe = 5;

    TickType_t interval = pdMS_TO_TICKS(chuKySafe * 1000);

    if ((now - lastPublish) >= interval) {
      lastPublish = now;

      // ===== Lấy giá trị theo cờ enable =====
      float tempToSend     = g_tempEnable     ? glob_temperature : 0.0f;
      float humidToSend    = g_humidEnable    ? glob_humidity    : 0.0f;
      float lightToSend    = g_lightEnable    ? glob_light       : 0.0f;
      float moistureToSend = g_moistureEnable ? glob_moisture    : 0.0f;

      // ===== Làm tròn =====
      float tempRounded     = roundf(tempToSend * 100.0f) / 100.0f;
      float humidRounded    = roundf(humidToSend * 100.0f) / 100.0f;
      int   lightInt        = (int)roundf(lightToSend);
      float moistureRounded = roundf(moistureToSend * 100.0f) / 100.0f;

      // ===== Đổi sang chuỗi =====
      char tempStr[16];
      char humidStr[16];
      char lightStr[16];
      char moistureStr[16];

      snprintf(tempStr, sizeof(tempStr), "%.2f", tempRounded);
      snprintf(humidStr, sizeof(humidStr), "%.2f", humidRounded);
      snprintf(lightStr, sizeof(lightStr), "%d", lightInt);
      snprintf(moistureStr, sizeof(moistureStr), "%.2f", moistureRounded);

      // ===== Publish feed riêng =====
      bool okTemp     = client.publish(topicTemp.c_str(), tempStr);
      bool okHumid    = client.publish(topicHumid.c_str(), humidStr);
      bool okLight    = client.publish(topicLight.c_str(), lightStr);
      bool okMoisture = client.publish(topicMoisture.c_str(), moistureStr);

      // ===== Publish JSON tổng hợp =====
      // bool okData = false;
      // char jsonBuffer[192] = {0};

      // if (g_data) {
      //   StaticJsonDocument<192> doc;
      //   doc["temperature"] = tempRounded;
      //   doc["humidity"]    = humidRounded;
      //   doc["light"]       = lightInt;
      //   doc["moisture"]    = moistureRounded;

      //   serializeJson(doc, jsonBuffer);
      //   okData = client.publish(topicData.c_str(), jsonBuffer, true);
      // }

      // ===== Debug Serial =====
      // Serial.println("==== PUBLISH =====");

      // Serial.print("TEMP     -> ");
      // Serial.print(tempStr);
      // Serial.print(" | ");
      // Serial.println(okTemp ? "OK" : "FAIL");

      // Serial.print("HUMID    -> ");
      // Serial.print(humidStr);
      // Serial.print(" | ");
      // Serial.println(okHumid ? "OK" : "FAIL");

      // Serial.print("LIGHT    -> ");
      // Serial.print(lightStr);
      // Serial.print(" | ");
      // Serial.println(okLight ? "OK" : "FAIL");

      // Serial.print("MOISTURE -> ");
      // Serial.print(moistureStr);
      // Serial.print(" | ");
      // Serial.println(okMoisture ? "OK" : "FAIL");

      // if (g_data) {
      //   Serial.print("JSON     -> ");
      //   Serial.println(jsonBuffer);
      //   Serial.print("DATA     -> ");
      //   Serial.println(okData ? "OK" : "FAIL");
      // } else {
      //   Serial.println("DATA     -> DISABLED");
      // }

      // Serial.print("Chu ky gui: ");
      // Serial.print(chuKySafe);
      // Serial.println(" giay");
      // Serial.println("========================");
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
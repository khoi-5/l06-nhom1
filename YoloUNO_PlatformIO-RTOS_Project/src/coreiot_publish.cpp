#include "coreiot.h"
#include <ArduinoJson.h>

// ================== BIẾN SENSOR TOÀN CỤC (đã có sẵn chỗ khác) ==================
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;


// ================== TASK CHÍNH GỬI/POLL MQTT ==================
void coreiot_task(void *pvParameters) {
  setup_coreiot();   // chờ WiFi + setup MQTT

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  TickType_t lastPublish = xTaskGetTickCount();

  while (1) {
    // 1) Đảm bảo MQTT luôn online
    if (!client.connected()) {
      reconnect();   // hàm reconnect cũ của bạn
    }
    client.loop();   // xử lý gói tin đến / ping

    // 2) Tính khoảng thời gian publish theo chu_ky (GIÂY)
    TickType_t now      = xTaskGetTickCount();
    TickType_t interval = pdMS_TO_TICKS(chu_ky * 1000);  // chu_ky: giây

    if (now - lastPublish >= interval) {
      lastPublish = now;

      // ----- Chọn giá trị gửi -----
      float tempToSend  = g_tempEnable  ? glob_temperature : 0.0f;
      float humidToSend = g_humidEnable ? glob_humidity    : 0.0f;
      float lightToSend = g_lightEnable ? glob_light       : 0.0f;

      float tempRounded  = roundf(tempToSend  * 100.0f) / 100.0f;
      float humidRounded = roundf(humidToSend * 100.0f) / 100.0f;
      int   lightInt     = (int)roundf(lightToSend);

      StaticJsonDocument<128> doc;
      char tempStr[10], humidStr[10];

      snprintf(tempStr,  sizeof(tempStr),  "%.2f", tempRounded);
      snprintf(humidStr, sizeof(humidStr), "%.2f", humidRounded);

      doc["temperature"] = tempStr;
      doc["humidity"]    = humidStr;
      doc["light"]       = lightInt;

      char buffer[128];
      size_t n = serializeJson(doc, buffer);
      client.publish(topicHTL.c_str(), buffer, n);

      Serial.print("Published JSON (chu_ky = ");
      Serial.print(chu_ky);
      Serial.print(" s) -> ");
      Serial.println(buffer);
      Serial.print(" ");
    }

    // 3) Nhịp lặp MQTT: ~100ms là được
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

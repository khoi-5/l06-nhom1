#include "coreiot.h"
#include <ArduinoJson.h>

// ================== BIẾN SENSOR TOÀN CỤC (đã có sẵn chỗ khác) ==================
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;


// ================== TASK CHÍNH GỬI/POLL MQTT ==================
void coreiot_task(void *pvParameters) {
  setup_coreiot();

  // set LED mặc định
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  const TickType_t delayTicks = pdMS_TO_TICKS(chu_ky * 1000); // ví dụ 10s

  while (1) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();  // xử lý MQTT, gọi mqttCallback khi có message

    // Chọn giá trị gửi lên tuỳ theo enable/disable
    float tempToSend  = g_tempEnable  ? glob_temperature : 0.0f;
    float humidToSend = g_humidEnable ? glob_humidity    : 0.0f;
    float lightToSend = g_lightEnable ? glob_light       : 0.0f;

    // ----- Làm tròn -----
    // temp & humid: 2 chữ số thập phân
    float tempRounded  = roundf(tempToSend  * 100.0f) / 100.0f;
    float humidRounded = roundf(humidToSend * 100.0f) / 100.0f;
    int   lightInt     = (int)roundf(lightToSend);

    // ===== Tạo JSON với string cho temp & humid =====
    StaticJsonDocument<128> doc;

    char tempStr[10];
    char humidStr[10];

    snprintf(tempStr,  sizeof(tempStr),  "%.2f", tempRounded);   // "25.10"
    snprintf(humidStr, sizeof(humidStr), "%.2f", humidRounded);  // "57.17"

    doc["temperature"] = tempStr;   // string
    doc["humidity"]    = humidStr;  // string
    doc["light"]       = lightInt;  // number

    char buffer[128];
    size_t n = serializeJson(doc, buffer);
    client.publish(topicHTL.c_str(), buffer, n);

    Serial.print("Published JSON -> ");
    Serial.print(topicHTL);
    Serial.print(" = ");
    Serial.println(buffer);
    Serial.print(" ");

    vTaskDelay(delayTicks);
  }
}

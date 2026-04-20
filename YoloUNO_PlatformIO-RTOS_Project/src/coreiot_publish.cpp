// #include "coreiot.h"
// #include <ArduinoJson.h>

// // ================== BIẾN SENSOR TOÀN CỤC (đã có sẵn chỗ khác) ==================
// extern float glob_temperature;
// extern float glob_humidity;
// extern float glob_light;


// // ================== TASK CHÍNH GỬI/POLL MQTT ==================
// // void coreiot_task(void *pvParameters) {
// //   setup_coreiot();   // chờ WiFi + setup MQTT

// //   pinMode(LED_PIN, OUTPUT);
// //   digitalWrite(LED_PIN, LOW);

// //   TickType_t lastPublish = xTaskGetTickCount();

// //   while (1) {
// //     // 1) Đảm bảo MQTT luôn online
// //     if (!client.connected()) {
// //       reconnect();   // hàm reconnect cũ của bạn
// //     }
// //     client.loop();   // xử lý gói tin đến / ping

// //     // 2) Tính khoảng thời gian publish theo chu_ky (GIÂY)
// //     TickType_t now      = xTaskGetTickCount();
// //     TickType_t interval = pdMS_TO_TICKS(chu_ky * 1000);  // chu_ky: giây

// //     if (now - lastPublish >= interval) {
// //       lastPublish = now;

// //       // ----- Chọn giá trị gửi -----
// //       float tempToSend  = g_tempEnable  ? glob_temperature : 0.0f;
// //       float humidToSend = g_humidEnable ? glob_humidity    : 0.0f;
// //       float lightToSend = g_lightEnable ? glob_light       : 0.0f;

// //       float tempRounded  = roundf(tempToSend  * 100.0f) / 100.0f;
// //       float humidRounded = roundf(humidToSend * 100.0f) / 100.0f;
// //       int   lightInt     = (int)roundf(lightToSend);

// //       StaticJsonDocument<128> doc;
// //       char tempStr[10], humidStr[10];

// //       snprintf(tempStr,  sizeof(tempStr),  "%.2f", tempRounded);
// //       snprintf(humidStr, sizeof(humidStr), "%.2f", humidRounded);

// //       doc["temperature"] = tempStr;
// //       doc["humidity"]    = humidStr;
// //       doc["light"]       = lightInt;

// //       char buffer[128];
// //       size_t n = serializeJson(doc, buffer);
// //       client.publish(topicHTL.c_str(), buffer, n);

// //       Serial.print("Published JSON (chu_ky = ");
// //       Serial.print(chu_ky);
// //       Serial.print(" s) -> ");
// //       Serial.println(buffer);
// //       Serial.print(" ");
// //     }

// //     // 3) Nhịp lặp MQTT: ~100ms là được
// //     vTaskDelay(pdMS_TO_TICKS(100));
// //   }
// // }

// void coreiot_task(void *pvParameters) {
//   setup_coreiot();   // chờ WiFi + setup MQTT

//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW);

//   TickType_t lastPublish = xTaskGetTickCount();

//   while (1) {
//     // 1) Đảm bảo MQTT luôn online
//     if (!client.connected()) {
//       reconnect();
//     }
//     client.loop();   // giữ kết nối MQTT sống

//     // 2) Tính khoảng thời gian publish theo chu_ky (GIÂY)
//     TickType_t now      = xTaskGetTickCount();
//     TickType_t interval = pdMS_TO_TICKS(chu_ky * 1000);

//     if (now - lastPublish >= interval) {
//       lastPublish = now;

//       // 3) Lấy giá trị cần gửi
//       float tempToSend  = g_tempEnable  ? glob_temperature : 0.0f;
//       float humidToSend = g_humidEnable ? glob_humidity    : 0.0f;
//       float lightToSend = g_lightEnable ? glob_light       : 0.0f;
//       float moistureToSend = glob_moisture? glob_moisture       : 0.0f;

//       // 4) Làm tròn dữ liệu
//       float tempRounded  = roundf(tempToSend  * 100.0f) / 100.0f;
//       float humidRounded = roundf(humidToSend * 100.0f) / 100.0f;
//       int   lightInt     = (int)roundf(lightToSend);
//       float moistureRounded = roundf(moistureToSend * 100.0f) / 100.0f;

//       // 5) Chuyển sang chuỗi để publish
//       char tempStr[16];
//       char humidStr[16];
//       char lightStr[16];
//       char moistureSrt[16];

//       snprintf(tempStr,  sizeof(tempStr),  "%.2f", tempRounded);
//       snprintf(humidStr, sizeof(humidStr), "%.2f", humidRounded);
//       snprintf(lightStr, sizeof(lightStr), "%d", lightInt);
//       snprintf(moistureSrt, sizeof(moistureSrt), "%.2f", moistureRounded);

//       // 6) Publish lên 3 feed riêng
//       bool okTemp  = client.publish(topicTemp.c_str(), tempStr);
//       bool okHumid = client.publish(topicHumid.c_str(), humidStr);
//       bool okLight = client.publish(topicLight.c_str(), lightStr);
//       bool okMoisture = client.publish(topicMoisture.c_str(), moistureSrt);


//       // ===== Tạo JSON tổng hợp =====
//         StaticJsonDocument<192> doc;
//         doc["temperature"] = glob_temperature;
//         doc["humidity"] = glob_humidity;
//         doc["light"] = glob_light;
//         doc["moisture"] = glob_moisture;

//         char jsonBuffer[192];
//         serializeJson(doc, jsonBuffer);

//         // ===== Publish lên dadn.data =====
//         bool ok = client.publish(topicData.c_str(), jsonBuffer, true);

//         Serial.print("Publish JSON -> ");
//         Serial.println(jsonBuffer);

//         if (ok) {
//           Serial.println("dadn.data publish OK");
//         } else {
//           Serial.println("dadn.data publish FAIL");
//         }

//       // 7) Debug Serial
//       // Serial.print("Publish TEMP  -> ");
//       // Serial.print(tempStr);
//       // Serial.print(" | ");
//       // Serial.println(okTemp ? "OK" : "FAIL");

//       // Serial.print("Publish HUMID -> ");
//       // Serial.print(humidStr);
//       // Serial.print(" | ");
//       // Serial.println(okHumid ? "OK" : "FAIL");

//       // Serial.print("Publish LIGHT -> ");
//       // Serial.print(lightStr);
//       // Serial.print(" | ");
//       // Serial.println(okLight ? "OK" : "FAIL");

//       // Serial.print("Chu ky gui: ");
//       // Serial.print(chu_ky);
//       // Serial.println(" giay");
//       // Serial.println(" ");
//     }

//     // 8) Nhịp lặp task
//     vTaskDelay(pdMS_TO_TICKS(100));
//   }
// }


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

// ================== TASK CHÍNH GỬI/POLL MQTT ==================
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
      bool okData = false;
      char jsonBuffer[192] = {0};

      if (g_data) {
        StaticJsonDocument<192> doc;
        doc["temperature"] = tempRounded;
        doc["humidity"]    = humidRounded;
        doc["light"]       = lightInt;
        doc["moisture"]    = moistureRounded;

        serializeJson(doc, jsonBuffer);
        okData = client.publish(topicData.c_str(), jsonBuffer, true);
      }

      // ===== Debug Serial =====
      Serial.println("===== MQTT PUBLISH =====");

      Serial.print("TEMP     -> ");
      Serial.print(tempStr);
      Serial.print(" | ");
      Serial.println(okTemp ? "OK" : "FAIL");

      Serial.print("HUMID    -> ");
      Serial.print(humidStr);
      Serial.print(" | ");
      Serial.println(okHumid ? "OK" : "FAIL");

      Serial.print("LIGHT    -> ");
      Serial.print(lightStr);
      Serial.print(" | ");
      Serial.println(okLight ? "OK" : "FAIL");

      Serial.print("MOISTURE -> ");
      Serial.print(moistureStr);
      Serial.print(" | ");
      Serial.println(okMoisture ? "OK" : "FAIL");

      if (g_data) {
        Serial.print("JSON     -> ");
        Serial.println(jsonBuffer);
        Serial.print("DATA     -> ");
        Serial.println(okData ? "OK" : "FAIL");
      } else {
        Serial.println("DATA     -> DISABLED");
      }

      Serial.print("Chu ky gui: ");
      Serial.print(chuKySafe);
      Serial.println(" giay");
      Serial.println("========================");
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
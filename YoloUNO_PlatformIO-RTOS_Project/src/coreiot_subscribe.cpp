#include "coreiot.h"
#include <ArduinoJson.h>

// ================== CALLBACK NHẬN TIN MQTT ==================
extern bool g_lightEnable;
extern bool g_tempEnable;
extern bool g_humidEnable;
extern int chu_ky;            // đơn vị: giây, dùng cho coreiot_task
extern String wifi_ssid;
extern String wifi_password;
void connectToWiFi();              // từ mainserver.cpp
void neo_toggle(int r, int g, int b, bool on);  // hàm điều khiển NeoPixel

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length);
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  String t = String(topic);

  Serial.print("MQTT msg ["); Serial.print(t); Serial.print("] ");
  Serial.println(msg);

  // ===== LED1 ON/OFF (chuỗi "ON"/"OFF") =====
  if (t.endsWith("/" + String(FEED_LED))) {
    pinMode(LED_PIN, OUTPUT);
    if (msg == "ON" || msg == "1") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED1: ON");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED1: OFF");
    }
  }

  // ===== Bật/tắt gửi sensor qua JSON {temperature,humidity,light} =====
  else if (t.endsWith("/" + String(FEED_HTL_CTRL))) {
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (err) {
      Serial.print("HTL_CTRL JSON error: ");
      Serial.println(err.c_str());
      return;
    }

    if (doc.containsKey("temperature")) {
      g_tempEnable = doc["temperature"].as<int>() != 0;
    }
    if (doc.containsKey("humidity")) {
      g_humidEnable = doc["humidity"].as<int>() != 0;
    }
    if (doc.containsKey("light")) {
      g_lightEnable = doc["light"].as<int>() != 0;
    }

    Serial.printf("Sensor enable -> T:%d H:%d L:%d\n",
                  g_tempEnable, g_humidEnable, g_lightEnable);
  }

  // ===== LED Neo: {"r":..,"g":..,"b":..,"on":0/1} =====
  else if (t.endsWith("/" + String(FEED_LED_NEO))) {
    StaticJsonDocument<128> doc;
    if (deserializeJson(doc, msg)) {
      Serial.println("LED_NEO JSON error");
      return;
    }
    int r = doc["r"] | 0;
    int g = doc["g"] | 0;
    int b = doc["b"] | 0;
    bool on = (doc["on"] | 1) != 0;

    r = constrain(r, 0, 255);
    g = constrain(g, 0, 255);
    b = constrain(b, 0, 255);

    Serial.printf("LED_NEO -> on=%d R=%d G=%d B=%d\n", on, r, g, b);
    neo_toggle(on ? r : 0, on ? g : 0, on ? b : 0, on);
  }

  // ===== Chu kỳ gửi (chu-ky): chuỗi số giây =====
  else if (t.endsWith("/" + String(FEED_CYCLE))) {
    uint32_t sec = msg.toInt();
    if (sec >= 5 && sec <= 300) {       // 5–300 giây
      chu_ky = sec;
      Serial.printf("Updated chu_ky = %u s\n", chu_ky);
    } else {
      Serial.println("chu_ky out of range");
    }
  }

  // ===== WiFi: {"ssid":"...","password":"..."} =====
  else if (t.endsWith("/" + String(FEED_WIFI))) {
    StaticJsonDocument<192> doc;
    if (deserializeJson(doc, msg)) {
      Serial.println("WIFI JSON error");
      return;
    }
    const char* s = doc["ssid"] | "";
    const char* p = doc["password"] | "";

    if (strlen(s) > 0) {
      wifi_ssid = String(s);
      wifi_password = String(p);
      Serial.printf("New WiFi cfg -> SSID=%s\n", wifi_ssid.c_str());
      connectToWiFi();   // dùng lại hàm bên mainserver
    }
  }
}
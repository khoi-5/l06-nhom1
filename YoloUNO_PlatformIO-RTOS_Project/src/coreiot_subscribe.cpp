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
  Serial.print(" ");

  // ===== LED1 ON/OFF (chuỗi "ON"/"OFF") =====
  if (t.endsWith("/" + String(FEED_LED))) {
    msg.trim();
    msg.toUpperCase();

    if (msg == "AUTO" || msg == "-1") {
      glob_led_cmd = -1;
      Serial.println("Received LED = AUTO");
      Serial.println(glob_led_cmd);
      Serial.print(" ");
    }
    else if (msg == "OFF" || msg == "0") {
      glob_led_cmd = 0;
      Serial.println("Received LED = OFF");
      Serial.println(glob_led_cmd);
      Serial.print(" ");
    }
    else if (msg == "ON" || msg == "1") {
      glob_led_cmd = 1;
      Serial.println("Received LED = ON");
      Serial.println(glob_led_cmd);
      Serial.print(" ");
    }
    else {
      Serial.print("Invalid LED command: ");
      Serial.println(msg);
      Serial.print(" ");
    }
  }

  // ===== Bật/tắt gửi sensor qua JSON {temperature,humidity,light} =====
  else if (t.endsWith("/" + String(FEED_HTL_CTRL))) {
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (err) {
      Serial.print("HTL_CTRL JSON error: ");
      Serial.println(err.c_str());
      Serial.print(" ");
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
    Serial.print(" ");
  }

  // ===== LED Neo: {"r":..,"g":..,"b":..,"on":0/1} =====
  else if (t.endsWith("/" + String(FEED_LED_NEO))) {
    StaticJsonDocument<128> doc;
    if (deserializeJson(doc, msg)) {
      Serial.println("LED_NEO JSON error");
      Serial.print(" ");
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
    Serial.print(" ");
    neo_toggle(on ? r : 0, on ? g : 0, on ? b : 0, on);
  }

  // ===== Chu kỳ gửi (chu-ky): chuỗi số giây =====
  else if (t.endsWith("/" + String(FEED_CYCLE))) {
    uint32_t sec = msg.toInt();
    if (sec >= 5 && sec <= 300) {       // 5–300 giây
      chu_ky = sec;
      Serial.printf("Updated chu_ky = %u s\n", chu_ky);
      Serial.print(" ");
    } else {
      Serial.println("chu_ky out of range");
      Serial.print(" ");
    }
  }

  // ===== WiFi: {"ssid":"...","password":"..."} =====
  else if (t.endsWith("/" + String(FEED_WIFI))) {
    StaticJsonDocument<192> doc;
    if (deserializeJson(doc, msg)) {
      Serial.println("WIFI JSON error");
      Serial.print(" ");
      return;
    }
    const char* s = doc["ssid"] | "";
    const char* p = doc["password"] | "";

    if (strlen(s) > 0) {
      wifi_ssid     = String(s);
      wifi_password = String(p);
      Serial.printf("New WiFi cfg -> SSID=%s\n", wifi_ssid.c_str());
      Serial.print(" ");

      //  QUAN TRỌNG: dùng chung logic timeout 10s như HTTP /connect
      isAPMode        = false;
      isWifiConnected = false;
      connecting      = true;
      connect_start_ms = millis();

      connectToWiFi();   // bắt đầu thử connect STA
    }
  }
  
  else if (t.endsWith("/" + String(FEED_FAN))) {
    msg.trim();
    msg.toUpperCase();

    if (msg == "AUTO" || msg == "-1") {
      glob_fan_cmd = -1;
      Serial.println("Received FAN = AUTO");
    }
    else if (msg == "OFF" || msg == "0") {
      glob_fan_cmd = 0;
      Serial.println("Received FAN = OFF");
    }
    else if (msg == "ON" || msg == "1") {
      glob_fan_cmd = 1;
      Serial.println("Received FAN = ON");
    }
    else {
      Serial.print("Invalid FAN command: ");
      Serial.println(msg);
    }
  }


  else if (t.endsWith("/" + String(FEED_MAYBOM))) {
    msg.trim();
    msg.toUpperCase();

    if (msg == "AUTO" || msg == "-1") {
      glob_maybom_cmd = -1;
      Serial.println("Received MAYBOM = AUTO");
      Serial.println(glob_maybom_cmd);
      Serial.print(" ");
    }
    else if (msg == "OFF" || msg == "0") {
      glob_maybom_cmd = 0;
      Serial.println("Received MAYBOM = OFF");
      Serial.println(glob_maybom_cmd);
      Serial.print(" ");
    }
    else if (msg == "ON" || msg == "1") {
      glob_maybom_cmd = 1;
      Serial.println("Received MAYBOM = ON");
      Serial.println(glob_maybom_cmd);
      Serial.print(" ");
    }
    else {
      Serial.print("Invalid MAYBOM command: ");
      Serial.println(msg);
      Serial.print(" ");
    }
  }

    else if (t.endsWith("/" + String(FEED_PHANTRAM))) {
        phantram = msg.toInt();

        if (phantram < 0) phantram = 0;
        if (phantram > 100) phantram = 100;

        Serial.print("Received PHANTRAM = ");
        Serial.println(phantram);
    }

}
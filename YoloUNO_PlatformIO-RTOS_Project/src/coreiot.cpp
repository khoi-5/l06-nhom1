#include <WiFi.h>
#include "coreiot.h"
#include "aio.h"
#include <PubSubClient.h>

// ========= Cấu hình Wi-Fi =========
// (phần WiFi của bạn để ở file khác + semaphore nên mình không đụng)

// ========= Adafruit IO =========
const char* AIO_SERVER    = "io.adafruit.com";
const int   mqttPort      = 1883;                    // test nhanh non-TLS
const char* AIO_USERNAME  = MY_AIO_USERNAME; 
const char* AIO_KEY = MY_AIO_KEY;

// ========= Tên feed =========
// Sensor
const char* FEED_TEMP     = "temperature";
const char* FEED_HUMID    = "humidity";
const char* FEED_LIGHT    = "light";
// Control
const char* FEED_LED      = "led";
const char* FEED_LIGHT_CTRL = "light_control";
const char* FEED_ENV_CTRL   = "humidity_temperature_control";

// ========= LED điều khiển thật trên board =========


// ========= Biến dữ liệu có sẵn =========
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;

// ========= Semaphore có sẵn =========
extern SemaphoreHandle_t xBinarySemaphoreInternet;

// ========= MQTT client =========
WiFiClient espClient;
PubSubClient client(espClient);

// Build topic đầy đủ  <username>/feeds/<feed>
String baseTopic      = String(AIO_USERNAME) + "/feeds/";
String topicTemp      = baseTopic + FEED_TEMP;
String topicHumid     = baseTopic + FEED_HUMID;
String topicLight     = baseTopic + FEED_LIGHT;
String topicLed       = baseTopic + FEED_LED;
String topicLightCtrl = baseTopic + FEED_LIGHT_CTRL;
String topicEnvCtrl   = baseTopic + FEED_ENV_CTRL;

// ========= Trạng thái control =========
// true  -> gửi giá trị sensor thật
// false -> gửi 0
bool g_lightEnable = true;   // điều khiển feed light
bool g_envEnable   = true;   // điều khiển feed temperature + humidity



// ========= MQTT reconnect =========
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to Adafruit IO... ");
    
    // Tạo Client ID bằng MAC Address + Số ngẫu nhiên
    String clientId = "ESP32Client-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    clientId += String(random(0xffff), HEX); // <--- DÒNG ĐÃ THÊM

    if (client.connect(clientId.c_str(), AIO_USERNAME, AIO_KEY)) {
      Serial.println("connected!");

      // Subscribe các feed điều khiển
      client.subscribe(topicLed.c_str());
      client.subscribe(topicLightCtrl.c_str());
      client.subscribe(topicEnvCtrl.c_str());

      Serial.print("Subscribed: "); Serial.println(topicLed);
      Serial.print("Subscribed: "); Serial.println(topicLightCtrl);
      Serial.print("Subscribed: "); Serial.println(topicEnvCtrl);
    } else {
      Serial.print("failed, rc="); Serial.print(client.state());
      Serial.println(" retry in 5 seconds");
      // Dùng vTaskDelay() thay vì delay() để tuân thủ FreeRTOS
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

// ========= Callback nhận tin =========
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println(msg);

  String t = String(topic);

  // ---- Điều khiển LED vật lý ----
  if (t.endsWith("/" + String(FEED_LED))) {
    if (msg == "ON" || msg == "1") {
      Serial.println("LED device turned ON.");
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println("LED device turned OFF.");
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, LOW);
    }
  }
  // ---- Điều khiển LIGHT (gửi 0 hoặc giá trị thật) ----
  else if (t.endsWith("/" + String(FEED_LIGHT_CTRL))) {
    if (msg == "ON" || msg == "1") {
      g_lightEnable = true;
      Serial.println("LIGHT control: ON (send real light value)");
    } else {
      g_lightEnable = false;
      Serial.println("LIGHT control: OFF (send 0)");
    }
  }
  // ---- Điều khiển HUMIDITY + TEMPERATURE ----
  else if (t.endsWith("/" + String(FEED_ENV_CTRL))) {
    if (msg == "ON" || msg == "1") {
      g_envEnable = true;
      Serial.println("ENV control: ON (send real temp & humid)");
    } else {
      g_envEnable = false;
      Serial.println("ENV control: OFF (send 0 for temp & humid)");
    }
  }
}

// ========= Setup mạng + MQTT (giữ tên hàm cũ) =========
void setup_coreiot() {
  // Chờ semaphore Internet sẵn có của bạn
  while (1) {
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  client.setServer(AIO_SERVER, mqttPort);
  client.setCallback(callback);
}

// ========= Task loop =========
void coreiot_task(void *pvParameters) {
  setup_coreiot();

  // set LED mặc định
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  const TickType_t delayTicks = pdMS_TO_TICKS(PUSHED_WAITING_TIME * 1000); // 10s

  while (1) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    // Chọn giá trị gửi lên tuỳ theo enable/disable
    float tempToSend  = g_envEnable   ? glob_temperature : 0.0f;
    float humidToSend = g_envEnable   ? glob_humidity    : 0.0f;
    float lightToSend = g_lightEnable ? glob_light       : 0.0f;

    client.publish(topicTemp.c_str(),  String(tempToSend).c_str());
    client.publish(topicHumid.c_str(), String(humidToSend).c_str());
    client.publish(topicLight.c_str(), String(lightToSend).c_str());

    Serial.print("Published -> "); Serial.print(topicTemp);
    Serial.print(" = "); Serial.println(tempToSend);

    Serial.print("Published -> "); Serial.print(topicHumid);
    Serial.print(" = "); Serial.println(humidToSend);

    Serial.print("Published -> "); Serial.print(topicLight);
    Serial.print(" = "); Serial.println(lightToSend);

    vTaskDelay(delayTicks);  // Publish every 10 seconds
  }
}

/* ===== Nếu muốn MQTT TLS sau khi test ổn =====
 * - Thay WiFiClient bằng WiFiClientSecure và setInsecure() (hoặc nạp root CA).
 *   Ví dụ:
 *     #include <WiFiClientSecure.h>
 *     WiFiClientSecure secureClient; secureClient.setInsecure();
 *     PubSubClient client(secureClient);
 *     const int mqttPort = 8883;
 */

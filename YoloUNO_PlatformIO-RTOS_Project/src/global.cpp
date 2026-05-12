#include "global.h"

float glob_temperature = 0;
float glob_humidity = 0;
float glob_light = 0;
float glob_moisture =0;

volatile float temp_temperature = 0.0f;
volatile float temp_humidity = 0.0f;
volatile int temp_light = 0;
volatile float temp_moisture = 0.0f;



volatile int glob_fan_cmd = 0;
volatile int glob_led_cmd = 0;
volatile int glob_maybom_cmd = 0;

volatile int phantram = 10;



int chu_ky = 10;

// ===== Adafruit IO config =====
const char* AIO_SERVER = "io.adafruit.com";
const int mqttPort = 1883;

// ===== AP mode / local config =====
String ssid = "dadn-nhom24";
String password = "12345678";

// ===== WiFi station config =====
String wifi_ssid = "701H6-KH&KTMT";
String wifi_password = "svkhktmt";

boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();

void init(){
    temp_temperature = 0;
    temp_humidity = 0;
    temp_light = 0;
    temp_moisture = 0;
}
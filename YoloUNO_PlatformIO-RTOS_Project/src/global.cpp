#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;
float glob_light = 0;
int chu_ky = 10;


// ================== CẤU HÌNH ADAFRUIT IO ==================
const char* AIO_SERVER   = "io.adafruit.com";
const int   mqttPort     = 1883;    

String ssid = "ESP32-nhom1-l06!!!";
String password = "12345678";
// String wifi_ssid = "ACLAB";
// String wifi_password= "ACLAB2023";
String wifi_ssid = "BACH YEN";
String wifi_password= "TU0918289939";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();


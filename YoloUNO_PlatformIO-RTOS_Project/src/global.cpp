#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;
float glob_light = 0;

String ssid = "ESP32-nhom1-l06!!!";
String password = "12345678";
String wifi_ssid = "ACLAB";
String wifi_password= "ACLAB2023";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();

int collected_waiting_time = 5;
int push_waiting_time = 5;
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ===== Global sensor data =====
extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;
extern float glob_moisture;

extern volatile float temp_temperature;
extern volatile float temp_humidity;
extern volatile int temp_light;
extern volatile float temp_moisture;




extern volatile int glob_fan_cmd;
extern volatile int glob_led_cmd;
extern volatile int glob_maybom_cmd;

extern volatile int phantram;

// ===== Adafruit IO config =====
extern const char* AIO_SERVER;
extern const int mqttPort;

// ===== WiFi config =====
extern String ssid;
extern String password;
extern String wifi_ssid;
extern String wifi_password;
extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;

// ===== Timing config =====
#define COLLECTED_WAITING_TIME 5
#define PUSHED_WAITING_TIME 10

extern int chu_ky;

void init();

#endif
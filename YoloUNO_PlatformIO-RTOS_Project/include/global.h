#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "light_monitor.h"
#include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"


extern float glob_temperature;
extern float glob_humidity;
extern float glob_light;

// ================== CẤU HÌNH ADAFRUIT IO ==================
extern const char* AIO_SERVER ;
extern const int   mqttPort;           

extern String ssid;
extern String password;
extern String wifi_ssid;
extern String wifi_password;
extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;

#define COLLECTED_WAITING_TIME 5
#define PUSHED_WAITING_TIME 10
extern int chu_ky;


#endif
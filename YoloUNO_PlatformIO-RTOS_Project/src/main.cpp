#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "light_monitor.h"
#include "soil_moisture_monitor.h"
#include "lcd_display.h"
#include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"
#include "fan_control.h"
#include "dadn_led.h"
#include "maybom.h"


void setup() {
  Serial.begin(115200);
  init();
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(light_monitor, "Task LIGHT Monitor" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(soil_moisture_monitor, "Task soil moisture Monitor" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(lcd_display_task, "Task LCD Display", 4096, NULL, 1, NULL);
  xTaskCreate(fan_control_task, "Task FAN Control", 2048, NULL, 2, NULL);
  xTaskCreate(led_control_task, "Task LED Control", 2048, NULL, 2, NULL);
  xTaskCreate(maybom_control_task, "Task MAYBOM Control", 2048, NULL, 2, NULL);
  xTaskCreate(main_server_task, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
}

void loop() {
  
}


// #include <Arduino.h>
// #include <Adafruit_NeoPixel.h>

// #define LED_PIN   6   // D4 = GPIO7
// #define LED_COUNT 4

// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   Serial.println("NeoPixel test start");

//   strip.begin();
//   strip.clear();
//   strip.show();
// }

// void loop() {
//   Serial.println("RED ON");
//   for (int i = 0; i < LED_COUNT; i++) {
//     strip.setPixelColor(i, strip.Color(255, 0, 0));
//   }
//   strip.show();
//   delay(2000);

//   Serial.println("OFF");
//   strip.clear();
//   strip.show();
//   delay(1000);

//   Serial.println("GREEN ON");
//   for (int i = 0; i < LED_COUNT; i++) {
//     strip.setPixelColor(i, strip.Color(0, 255, 0));
//   }
//   strip.show();
//   delay(2000);

//   Serial.println("OFF");
//   strip.clear();
//   strip.show();
//   delay(1000);
// }
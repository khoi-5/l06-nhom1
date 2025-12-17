#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "light_monitor.h"
#include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"


void setup() {
  Serial.begin(115200);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(light_monitor, "Task LIGHT Monitor" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(main_server_task, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
}

void loop() {
  
}
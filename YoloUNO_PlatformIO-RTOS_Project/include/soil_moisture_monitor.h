#ifndef __SOIL_MOISTURE_MONITOR_H__
#define __SOIL_MOISTURE_MONITOR_H__

#include <Arduino.h>
#include "global.h"

#define SOIL_SENSOR_PIN 3  

void soil_moisture_monitor(void *pvParameters);

#endif
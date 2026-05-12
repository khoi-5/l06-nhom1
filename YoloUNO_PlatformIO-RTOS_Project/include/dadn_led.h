#ifndef __DADN_LED_H__
#define __DADN_LED_H__

#include <Arduino.h>
#include "global.h"

void led_control_task(void *pvParameters);
void led_on();
void led_off();

#endif
#ifndef __FAN_CONTROL_H__
#define __FAN_CONTROL_H__

#include <Arduino.h>
#include "global.h"

// ===== TASK =====
void fan_control_task(void *pvParameters);

// ===== ĐIỀU KHIỂN QUẠT =====
void fan_on();
void fan_off();

// ===== TEST LED =====
void fan_led_on();
void fan_led_off();

#endif
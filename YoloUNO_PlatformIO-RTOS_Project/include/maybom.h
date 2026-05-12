#ifndef __MAYBOM_H__
#define __MAYBOM_H__

#include <Arduino.h>
#include "global.h"

// ===== TASK =====
void maybom_control_task(void *pvParameters);

// ===== ON / OFF =====
void maybom_on();
void maybom_off();

// ===== TEST LED =====
void maybom_led_on();
void maybom_led_off();

// ===== CHẠY THEO % =====
void maybom_run_percent(int percent);

// ===== (OPTIONAL) BIẾN % TOÀN CỤC =====
extern volatile int glob_maybom_percent;

#endif
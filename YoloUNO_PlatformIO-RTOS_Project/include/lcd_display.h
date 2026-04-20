#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "global.h"

void lcd_display_task(void *pvParameters);

#endif
#include "lcd_display.h"
#include <math.h>

LiquidCrystal_I2C lcd(0x21, 16, 2);

void lcd_display_task(void *pvParameters) {
    Wire.begin(11, 12);

    lcd.begin();
    lcd.backlight();
    lcd.clear();

    while (1) {
        float rt = roundf(glob_temperature * 10.0f) / 10.0f;
        int rh  = (int)roundf(glob_humidity);
        int lux = (int)roundf(glob_light);
        int sm  = (int)roundf(glob_moisture);

        // Xóa toàn bộ trước để tránh rác ký tự cũ
        lcd.clear();

        // ===== DÒNG 0 =====
        lcd.setCursor(0, 0);
        lcd.print("RT:");

        lcd.setCursor(3, 0);
        if (rt < 10) lcd.print(" ");
        lcd.print(rt);

        lcd.setCursor(7, 0);
        lcd.print((char)223);
        lcd.print("C");

        lcd.setCursor(10, 0);
        lcd.print("RH:");

        lcd.setCursor(13, 0);
        if (rh < 10) lcd.print(" ");
        lcd.print(rh);

        lcd.setCursor(15, 0);
        lcd.print("%");

        // ===== DÒNG 1 =====
        lcd.setCursor(0, 1);
        lcd.print("LUX:");

        lcd.setCursor(4, 1);
        if (lux < 1000) lcd.print(" ");
        if (lux < 100)  lcd.print(" ");
        if (lux < 10)   lcd.print(" ");
        lcd.print(lux);

        lcd.setCursor(10, 1);
        lcd.print("SM:");

        lcd.setCursor(13, 1);
        if (sm < 10) lcd.print(" ");
        lcd.print(sm);

        lcd.setCursor(15, 1);
        lcd.print("%");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
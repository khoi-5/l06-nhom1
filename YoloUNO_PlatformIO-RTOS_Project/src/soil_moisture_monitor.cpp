#include "soil_moisture_monitor.h"
#include <math.h>

extern float glob_moisture;
extern int chu_ky;

void soil_moisture_monitor(void *pvParameters) {

    pinMode(SOIL_SENSOR_PIN, INPUT);

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sum = 0.0f;
    int count = 0;
    int elapsed = 0;

    while (1) {

        // Đọc giá trị analog 0–4095
        int raw = analogRead(SOIL_SENSOR_PIN);

        // Đổi analog 0–4095 sang phần trăm 0–100%
        float moisture = ((float)raw / 4095.0f) * 100.0f;

        // Giới hạn về 0–100% cho chắc
        if (moisture < 0.0f) {
            moisture = 0.0f;
        } else if (moisture > 100.0f) {
            moisture = 100.0f;
        }

        // Cộng vào để tính trung bình
        sum += moisture;
        count++;
        elapsed += SAMPLE_PERIOD_SEC;

        // Đủ chu kỳ thì cập nhật giá trị trung bình
        if (elapsed >= chu_ky) {
            float avg = (count > 0) ? (sum / count) : 0.0f;

            // Làm tròn 2 chữ số thập phân
            avg = roundf(avg * 100.0f) / 100.0f;

            glob_moisture = avg;

            Serial.print("[SOIL] raw=");
            Serial.print(raw);
            Serial.print(" | avg moisture=");
            Serial.print(glob_moisture, 2);
            Serial.println("%");

            // Reset bộ đếm chu kỳ
            sum = 0.0f;
            count = 0;
            elapsed = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
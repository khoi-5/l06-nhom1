#include "soil_moisture_monitor.h"
#include <math.h>

extern float glob_moisture;
extern int chu_ky;
extern volatile float temp_moisture;  // giống temp_light

void soil_moisture_monitor(void *pvParameters) {

    pinMode(SOIL_SENSOR_PIN, INPUT);

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sum = 0.0f;
    int count = 0;
    int elapsed = 0;

    while (1) {

        int raw = analogRead(SOIL_SENSOR_PIN);

        // convert 0–4095 → 0–100 (%)
        float rawMoisture = (1.0f - (float)raw / 4095.0f) * 100.0f;

        bool missData = false;  // bạn có thể nâng cấp sau

        float validMoisture;
        String status;

        if (missData) {
            validMoisture = temp_moisture;
            status = "MISS -> use temp";
        } else {
            // clamp theo spec 0–100%
            if (rawMoisture < 0.0f) {
                validMoisture = 0.0f;
                status = "CLAMP LOW";
            } else if (rawMoisture > 100.0f) {
                validMoisture = 100.0f;
                status = "CLAMP HIGH";
            } else {
                validMoisture = rawMoisture;
                status = "OK";
            }

            temp_moisture = validMoisture;
        }

        //  Debug mỗi lần đọc
        // Serial.println(
        //     "[SOIL] raw=" + String(raw) +
        //     " | rawMoist=" + String(rawMoisture, 2) +
        //     " | valid=" + String(validMoisture, 2) +
        //     " | temp=" + String(temp_moisture, 2) +
        //     " | status=" + status
        // );

        sum += validMoisture;
        count++;
        elapsed += SAMPLE_PERIOD_SEC;

        if (elapsed >= chu_ky) {
            float avg = (count > 0) ? sum / count : 0.0f;

            avg = roundf(avg * 100.0f) / 100.0f;
            glob_moisture = avg;

            //  Debug cuối chu kỳ
            // Serial.println("===== SOIL PERIOD DONE =====");
            // Serial.print("Period: ");
            // Serial.print(chu_ky);
            // Serial.println(" s");

            // Serial.print("Samples: ");
            // Serial.println(count);

            // Serial.print("Average Moisture: ");
            // Serial.print(avg, 2);
            // Serial.println(" %");

            // Serial.println("============================");
            // Serial.println();

            sum = 0.0f;
            count = 0;
            elapsed = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
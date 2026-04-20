#include "light_monitor.h"
#include <math.h>

extern float glob_light;
extern int chu_ky;
extern volatile int temp_light;

void light_monitor(void* pvParameters) {
    pinMode(LIGHT_SENSOR, INPUT);

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sumLight = 0.0f;
    int sampleCount = 0;
    int elapsedTime = 0;

    while (1) {
        int sensorValue = analogRead(LIGHT_SENSOR);

        
        bool missData = false;

        int validLight;
        String status;

        if (missData) {
            validLight = temp_light;
            status = "MISS -> use temp";
        } else {
            if (sensorValue < 0) {
                validLight = 0;
                status = "CLAMP LOW";
            } else if (sensorValue > 4095) {
                validLight = 4095;
                status = "CLAMP HIGH";
            } else {
                validLight = sensorValue;
                status = "OK";
            }

            temp_light = validLight;
        }

        //  Debug log mỗi lần đọc
        // Serial.println(
        // "[LIGHT] raw=" + String(sensorValue) +
        // " | valid=" + String(validLight) +
        // " | temp=" + String(temp_light) +
        // " | status=" + status
        // );

        sumLight += (float)validLight;
        sampleCount++;
        elapsedTime += SAMPLE_PERIOD_SEC;

        if (elapsedTime >= chu_ky) {
            float avgLight = 0.0f;

            if (sampleCount > 0) {
                avgLight = sumLight / sampleCount;
            }

            int avgLightInt = (int)roundf(avgLight);
            glob_light = (float)avgLightInt;

            //  Debug log cuối chu kỳ
            // Serial.println("===== LIGHT PERIOD DONE =====");
            // Serial.print("Period: ");
            // Serial.print(chu_ky);
            // Serial.println(" s");

            // Serial.print("Samples: ");
            // Serial.println(sampleCount);

            // Serial.print("Average Light: ");
            // Serial.println(avgLightInt);
            // Serial.println("=============================");
            // Serial.println();

            sumLight = 0.0f;
            sampleCount = 0;
            elapsedTime = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}





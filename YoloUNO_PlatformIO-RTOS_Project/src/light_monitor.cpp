#include "light_monitor.h"
#include <math.h>

extern float glob_light;
extern int chu_ky;

void light_monitor(void* pvParameters) {
    pinMode(LIGHT_SENSOR, INPUT);

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sum_Light = 0.0f;
    int count = 0;

    while (1) {
        int light = analogRead(LIGHT_SENSOR);

        // Giới hạn giá trị analog trong khoảng 0 - 4095
        if (light < 0) {
            light = 0;
        } else if (light > 4095) {
            light = 4095;
        }

        sum_Light += (float)light;
        count += 1;

        if (count >= chu_ky) {
            float avg_Light = sum_Light / count;

            // Làm tròn
            avg_Light = roundf(avg_Light);

            glob_light = avg_Light;

            sum_Light = 0.0f;
            count = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
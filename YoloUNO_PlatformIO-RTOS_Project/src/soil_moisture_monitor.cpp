#include "soil_moisture_monitor.h"
#include <math.h>

extern float glob_moisture;
extern int chu_ky;

void soil_moisture_monitor(void *pvParameters) {

    pinMode(SOIL_SENSOR_PIN, INPUT);

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sum_Moisture = 0.0f;
    int count = 0;

    while (1) {

        
        int raw = analogRead(SOIL_SENSOR_PIN);

        
        float moisture = ((float)raw / 4095.0f) * 100.0f;

       
        if (moisture < 0.0f) {
            moisture = 0.0f;
        } else if (moisture > 100.0f) {
            moisture = 100.0f;
        }

        
        sum_Moisture += moisture;
        count += 1;

        
        if (count >= chu_ky) {
            float avg_Moisture = sum_Moisture / count;

            
            avg_Moisture = roundf(avg_Moisture * 100.0f) / 100.0f;

            glob_moisture = avg_Moisture;

            // print
            // Serial.print("[SOIL] raw=");
            // Serial.print(raw);
            // Serial.print(" | avg_Moisture=");
            // Serial.print(glob_moisture, 2);
            // Serial.println("%");

            
            sum_Moisture = 0.0f;
            count = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
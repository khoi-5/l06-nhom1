#include "light_monitor.h"


extern float glob_light;

void light_monitor(void* pvParameters){
    pinMode(LIGHT_SENSOR, INPUT);

    const uint32_t SAMPLE_PERIOD = COLLECTED_WAITING_TIME; // = 5
    const int MAX_SAMPLES = 12;

    float lightBuf[MAX_SAMPLES] = {0};
    int idx    = 0;
    int filled = 0;

    while(1){
        int sensorValue = analogRead(LIGHT_SENSOR);
        float light = (float)sensorValue;

        // lưu vào buffer vòng tròn
        lightBuf[idx] = light;
        idx = (idx + 1) % MAX_SAMPLES;
        if (filled < MAX_SAMPLES) filled++;

        int windowSamples = chu_ky / SAMPLE_PERIOD;
        if (windowSamples > filled) windowSamples = filled;

        if (windowSamples > 0) {
            float sumL = 0.0f;
            for (int i = 0; i < windowSamples; i++) {
                int j = (idx - 1 - i + MAX_SAMPLES) % MAX_SAMPLES;
                sumL += lightBuf[j];
            }

            float avgLight = sumL / windowSamples;

            // ===== LÀM TRÒN VỀ INT =====
            int avgLightInt = (int)roundf(avgLight);

            glob_light = (float)avgLightInt;

            Serial.print("Light raw: ");
            Serial.print(sensorValue);
            Serial.print(" | AVG(");
            Serial.print(windowSamples);
            Serial.print(" samples): ");
            Serial.println(avgLightInt);
            Serial.println(" ");
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD * 1000));
    }
}

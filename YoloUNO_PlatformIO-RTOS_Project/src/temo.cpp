// #include "light_monitor.h"
// #include <math.h>

// extern float glob_light;
// extern int chu_ky;
// extern volatile int temp_light;
// extern volatile bool light_error_flag;

// void light_monitor(void* pvParameters) {
//     pinMode(LIGHT_SENSOR, INPUT);

//     const uint32_t SAMPLE_PERIOD_SEC = 1;

//     float sumLight = 0.0f;
//     int sampleCount = 0;
//     int elapsedTime = 0;

//     int prevLight = -1;
//     int unstableCount = 0;

//     while (1) {
//         int sensorValue = analogRead(LIGHT_SENSOR);
//         sensorValue = constrain(sensorValue, 0, 4095);

//         int diff = 0;
//         if (prevLight != -1) {
//             diff = abs(sensorValue - prevLight);
//         }

//         // Detect floating: nhảy quá mạnh liên tiếp
//         if (prevLight != -1 && diff > 300) {
//             unstableCount++;
//         } else {
//             unstableCount = 0;
//         }

//         int validLight;
//         String status;

//         if (unstableCount >= 3) {
//             light_error_flag = true;
//             temp_light = 0;
//             validLight = 0;
//             status = "ERROR_FLOAT";
//         } else {
//             light_error_flag = false;
//             validLight = sensorValue;
//             temp_light = validLight;
//             status = "OK";
//         }

//         prevLight = sensorValue;

//         Serial.println(
//             "[LIGHT] raw=" + String(sensorValue) +
//             " | valid=" + String(validLight) +
//             " | temp=" + String(temp_light) +
//             " | diff=" + String(diff) +
//             " | unstableCount=" + String(unstableCount) +
//             " | error=" + String(light_error_flag) +
//             " | status=" + status
//         );

//         sumLight += (float)validLight;
//         sampleCount++;
//         elapsedTime += SAMPLE_PERIOD_SEC;

//         if (elapsedTime >= chu_ky) {
//             float avgLight = (sampleCount > 0) ? (sumLight / sampleCount) : 0.0f;
//             int avgLightInt = (int)roundf(avgLight);
//             glob_light = (float)avgLightInt;

//             Serial.println("===== LIGHT PERIOD DONE =====");
//             Serial.print("Period: ");
//             Serial.print(chu_ky);
//             Serial.println(" s");
//             Serial.print("Samples: ");
//             Serial.println(sampleCount);
//             Serial.print("Average Light: ");
//             Serial.println(avgLightInt);
//             Serial.print("ERROR FLAG: ");
//             Serial.println(light_error_flag);
//             Serial.println("=============================");
//             Serial.println();

//             sumLight = 0.0f;
//             sampleCount = 0;
//             elapsedTime = 0;
//         }

//         vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
//     }
// }
#include "temp_humi_monitor.h"
#include <math.h>

DHT20 dht20;

extern float glob_temperature;
extern float glob_humidity;
extern int chu_ky;

// lưu giá trị hợp lệ gần nhất
float last_temperature = 0.0f;
float last_humidity    = 0.0f;

void temp_humi_monitor(void *pvParameters) {
    Wire.begin(11, 12);
    dht20.begin();

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sumTemp = 0.0f;
    float sumHumi = 0.0f;
    int sampleCount = 0;

    while (1) {
        dht20.read();

        float rawTemperature = dht20.getTemperature();
        float rawHumidity    = dht20.getHumidity();

        bool missData = isnan(rawTemperature) || isnan(rawHumidity);

        float validTemperature;
        float validHumidity;

        String tempStatus;
        String humiStatus;

        if (missData) {
            validTemperature = last_temperature;
            validHumidity    = last_humidity;

            tempStatus = "MISS -> use last";
            humiStatus = "MISS -> use last";
        } else {
            // clamp nhiệt độ theo spec: -40 ~ 80
            if (rawTemperature < -40.0f) {
                validTemperature = -40.0f;
                tempStatus = "CLAMP LOW";
            } else if (rawTemperature > 80.0f) {
                validTemperature = 80.0f;
                tempStatus = "CLAMP HIGH";
            } else {
                validTemperature = rawTemperature;
                tempStatus = "OK";
            }

            // clamp độ ẩm theo spec: 0 ~ 100
            if (rawHumidity < 0.0f) {
                validHumidity = 0.0f;
                humiStatus = "CLAMP LOW";
            } else if (rawHumidity > 100.0f) {
                validHumidity = 100.0f;
                humiStatus = "CLAMP HIGH";
            } else {
                validHumidity = rawHumidity;
                humiStatus = "OK";
            }

            last_temperature = validTemperature;
            last_humidity    = validHumidity;
        }

        // Debug log mỗi lần đọc
        // Serial.println(
        //     "[DHT20] rawT=" + String(rawTemperature, 2) +
        //     " | validT=" + String(validTemperature, 2) +
        //     " | lastT=" + String(last_temperature, 2) +
        //     " | tempStatus=" + tempStatus +
        //     " || rawH=" + String(rawHumidity, 2) +
        //     " | validH=" + String(validHumidity, 2) +
        //     " | lastH=" + String(last_humidity, 2) +
        //     " | humiStatus=" + humiStatus
        // );

        sumTemp += validTemperature;
        sumHumi += validHumidity;
        sampleCount++;

        if (sampleCount >= chu_ky) {
            float avgTemp = 0.0f;
            float avgHumi = 0.0f;

            if (sampleCount > 0) {
                avgTemp = sumTemp / sampleCount;
                avgHumi = sumHumi / sampleCount;
            }

            avgTemp = roundf(avgTemp * 100.0f) / 100.0f;
            avgHumi = roundf(avgHumi * 100.0f) / 100.0f;

            glob_temperature = avgTemp;
            glob_humidity    = avgHumi;

            // Debug log cuối chu kỳ
            // Serial.println("===== TEMP/HUMI PERIOD DONE =====");
            // Serial.print("Period: ");
            // Serial.print(chu_ky);
            // Serial.println(" s");

            // Serial.print("Samples: ");
            // Serial.println(sampleCount);

            // Serial.print("Average Temperature: ");
            // Serial.print(avgTemp, 2);
            // Serial.println(" C");

            // Serial.print("Average Humidity: ");
            // Serial.print(avgHumi, 2);
            // Serial.println(" %RH");

            // Serial.println("=================================");
            // Serial.println();

            sumTemp = 0.0f;
            sumHumi = 0.0f;
            sampleCount = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
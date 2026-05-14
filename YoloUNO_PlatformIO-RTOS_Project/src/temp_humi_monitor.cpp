#include "temp_humi_monitor.h"
#include <math.h>

DHT20 dht20;

extern float glob_temperature;
extern float glob_humidity;
extern int chu_ky;

void temp_humi_monitor(void *pvParameters) {
    Wire.begin(11, 12);
    dht20.begin();

    const uint32_t SAMPLE_PERIOD_SEC = 1;

    float sum_Temp = 0.0f;
    float sum_Humi = 0.0f;
    int count = 0;

    while (1) {
        dht20.read();

        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

        // Nếu đọc lỗi NaN thì bỏ qua mẫu này
        if (!isnan(temperature) && !isnan(humidity)) {

            // Giới hạn nhiệt độ theo ngưỡng DHT20: -40 đến 80 độ C
            if (temperature < -40.0f) {
                temperature = -40.0f;
            } else if (temperature > 80.0f) {
                temperature = 80.0f;
            }

            // Giới hạn độ ẩm theo ngưỡng: 0 đến 100%
            if (humidity < 0.0f) {
                humidity = 0.0f;
            } else if (humidity > 100.0f) {
                humidity = 100.0f;
            }

            sum_Temp += temperature;
            sum_Humi += humidity;
            count += 1;
        }

        if (count >= chu_ky) {
            float avg_Temp = sum_Temp / count;
            float avg_Humi = sum_Humi / count;

            avg_Temp = roundf(avg_Temp * 100.0f) / 100.0f;
            avg_Humi = roundf(avg_Humi * 100.0f) / 100.0f;

            glob_temperature = avg_Temp;
            glob_humidity = avg_Humi;

            sum_Temp = 0.0f;
            sum_Humi = 0.0f;
            count = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_SEC * 1000));
    }
}
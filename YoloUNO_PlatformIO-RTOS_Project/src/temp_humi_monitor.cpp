#include "temp_humi_monitor.h"

DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);

// từ chỗ khác
extern float glob_temperature;
extern float glob_humidity;

// hằng: chu kỳ lấy mẫu (giây)
const uint32_t SAMPLE_PERIOD = COLLECTED_WAITING_TIME;  // = 5

// Giả sử chu_ky max ~ 60s -> max 60 / 5 = 12 mẫu
// Nếu sau này chu_ky lớn hơn thì tăng MAX_SAMPLES lên
const int MAX_SAMPLES = 12;

void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    float tempBuf[MAX_SAMPLES]  = {0};
    float humidBuf[MAX_SAMPLES] = {0};
    int idx    = 0;   // vị trí ghi tiếp theo (0..MAX_SAMPLES-1)
    int filled = 0;   // số mẫu hiện có

    while (1){
        // ===== 1. Đọc cảm biến =====
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity    = dht20.getHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            temperature = humidity = -1;
        }

        // ===== 2. Ghi vào buffer vòng tròn =====
        tempBuf[idx]  = temperature;
        humidBuf[idx] = humidity;
        idx = (idx + 1) % MAX_SAMPLES;
        if (filled < MAX_SAMPLES) filled++;

        // ===== 3. Số mẫu cần để tính average = chu_ky / 5 =====
        int windowSamples = chu_ky / SAMPLE_PERIOD;   // đảm bảo nguyên vì chu_ky là bội số 5

        // Lúc mới chạy có thể chưa đủ windowSamples mẫu → lấy bấy nhiêu mẫu đang có
        if (windowSamples > filled) {
            windowSamples = filled;
        }

        // Tránh chia cho 0 khi vừa khởi động (chưa có mẫu nào hợp lệ)
        if (windowSamples > 0) {
            // ===== 4. Tính trung bình windowSamples mẫu gần nhất =====
            float sumT = 0.0f;
            float sumH = 0.0f;

            for (int i = 0; i < windowSamples; i++) {
                int j = (idx - 1 - i + MAX_SAMPLES) % MAX_SAMPLES; // lùi ngược từ mẫu mới nhất
                sumT += tempBuf[j];
                sumH += humidBuf[j];
            }

            float avgTemp  = sumT / windowSamples;
            float avgHumid = sumH / windowSamples;

            // ===== 5. Làm tròn 2 chữ số thập phân =====
            avgTemp  = roundf(avgTemp  * 100.0f) / 100.0f;
            avgHumid = roundf(avgHumid * 100.0f) / 100.0f;

            // ===== 6. Cập nhật biến global: coreiot_task sẽ publish giá trị này =====
            glob_temperature = avgTemp;
            glob_humidity    = avgHumid;

            // Debug
            Serial.print("RAW -> H: ");
            Serial.print(humidity);
            Serial.print("%  T: ");
            Serial.print(temperature);
            Serial.print("°C | AVG(");
            Serial.print(windowSamples);
            Serial.print(" samples) -> H: ");
            Serial.print(avgHumid);
            Serial.print("%  T: ");
            Serial.print(avgTemp);
            Serial.println("°C");
            Serial.println(" ");
        }

        // ===== 7. Đợi 5s rồi đọc tiếp =====
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD * 1000));
    }
}

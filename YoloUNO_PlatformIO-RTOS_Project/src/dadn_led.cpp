#include "dadn_led.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN 8
#define LED_COUNT 4
#define LIGHT_THRESHOLD 500

extern volatile int glob_led_cmd;
extern float glob_light;

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void led_on() {
    for (int i = 0; i < LED_COUNT; i++) {
        ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0)); // đỏ
    }
    ledStrip.show();
    Serial.println("LED ON");
}

void led_off() {
    ledStrip.clear();
    ledStrip.show();
    Serial.println("LED OFF");
}

void led_control_task(void *pvParameters) {
    ledStrip.begin();
    ledStrip.clear();
    ledStrip.show();

    int lastState = -1;

    // An toàn lúc mới khởi động
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        int currentState = 0;

        if (glob_led_cmd == 1) {
            // Bật thủ công
            currentState = 1;
        }
        else if (glob_led_cmd == 0) {
            // Tắt thủ công
            currentState = 0;
        }
        else {
            // Tự động theo ánh sáng
            if (glob_light > 0 && glob_light < LIGHT_THRESHOLD) {
                currentState = 1;
            } else {
                currentState = 0;
            }
        }

        if (currentState != lastState) {
            if (currentState == 1) {
                led_on();
            } else {
                led_off();
            }

            lastState = currentState;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
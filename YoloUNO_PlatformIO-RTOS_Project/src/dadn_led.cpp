#include "dadn_led.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN 8
#define LED_COUNT 4

extern volatile int glob_led_cmd;

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void led_control_task(void *pvParameters) {
    ledStrip.begin();
    ledStrip.clear();
    ledStrip.show();

    int lastState = -1;
    if (glob_led_cmd == 1) Serial.println("nhan dc tin hieu trung ===============");
    while (1) {
        int cmd = glob_led_cmd;

        if (cmd != lastState) {

            if (cmd == 0) {
                // 🔴 Tắt LED
                ledStrip.clear();
                ledStrip.show();
                Serial.println("LED OFF");
            } else {
                // 🟢 Bật LED (màu đỏ)
                for (int i = 0; i < LED_COUNT; i++) {
                    ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
                }
                ledStrip.show();
                Serial.println("LED ON (RED)");
            }

            lastState = cmd;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
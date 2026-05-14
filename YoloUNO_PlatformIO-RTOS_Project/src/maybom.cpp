#include "maybom.h"
#include <Adafruit_NeoPixel.h>

#define MAYBOM_PIN 10
#define MAYBOM_LED_PIN 9
#define LED_COUNT 4
#define MAYBOM_PERIOD_MS 1000
#define MOISTURE_THRESHOLD 10

extern volatile int glob_maybom_cmd;
extern volatile int phantram;
extern float glob_moisture;

Adafruit_NeoPixel maybomStrip(LED_COUNT, MAYBOM_LED_PIN, NEO_GRB + NEO_KHZ800);

int maybom_percent = 20;

void maybom_on() {
    digitalWrite(MAYBOM_PIN, HIGH);
}

void maybom_off() {
    digitalWrite(MAYBOM_PIN, LOW);
}

void maybom_led_on() {
    for (int i = 0; i < LED_COUNT; i++) {
        maybomStrip.setPixelColor(i, maybomStrip.Color(0, 255, 0));
    }
    maybomStrip.show();
}

void maybom_led_off() {
    maybomStrip.clear();
    maybomStrip.show();
}

void maybom_run_percent(int percent) {
    if (percent <= 0) {
        maybom_off();
        vTaskDelay(pdMS_TO_TICKS(MAYBOM_PERIOD_MS));
        return;
    }

    if (percent >= 100) {
        maybom_on();
        vTaskDelay(pdMS_TO_TICKS(MAYBOM_PERIOD_MS));
        return;
    }

    int onTime = (MAYBOM_PERIOD_MS * percent) / 100;
    int offTime = MAYBOM_PERIOD_MS - onTime;

    maybom_on();
    vTaskDelay(pdMS_TO_TICKS(onTime));

    maybom_off();
    vTaskDelay(pdMS_TO_TICKS(offTime));
}

void maybom_control_task(void *pvParameters) {
    pinMode(MAYBOM_PIN, OUTPUT);

    maybomStrip.begin();
    maybomStrip.clear();
    maybomStrip.show();

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        int currentState = 0;

        // maybom_percent = phantram;
        maybom_percent = 20;

        if (maybom_percent < 0) {
            maybom_percent = 0;
        }
        if (maybom_percent > 100) {
            maybom_percent = 100;
        }

        if (glob_maybom_cmd == 1) {
            // Bật thủ công
            currentState = 1;
        }
        else if (glob_maybom_cmd == 0) {
            // Tắt thủ công
            currentState = 0;
        }
        else {
            // Tự động theo độ ẩm đất
            if (glob_moisture < MOISTURE_THRESHOLD) {
                currentState = 1;
            } else {
                currentState = 0;
            }
        }

        if (currentState == 1) {
            maybom_led_on();
            maybom_run_percent(maybom_percent);
        } else {
            maybom_off();
            maybom_led_off();
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
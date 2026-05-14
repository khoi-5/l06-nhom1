#include "fan_control.h"
#include <Adafruit_NeoPixel.h>

#define FAN_PIN 6
#define FAN_LED_PIN 7
#define LED_COUNT 4
#define TEMP_THRESHOLD 35

extern volatile int glob_fan_cmd;
extern float glob_temperature;

Adafruit_NeoPixel fanStrip(LED_COUNT, FAN_LED_PIN, NEO_GRB + NEO_KHZ800);

void fan_on() {
    int pwm = map(70, 0, 100, 0, 255);
    analogWrite(FAN_PIN, pwm);
    Serial.println("FAN ON");
}

void fan_off() {
    analogWrite(FAN_PIN, 0);
    Serial.println("FAN OFF");
}

void fan_led_on() {
    for (int i = 0; i < LED_COUNT; i++) {
        fanStrip.setPixelColor(i, fanStrip.Color(0, 0, 255));
    }
    fanStrip.show();
}

void fan_led_off() {
    fanStrip.clear();
    fanStrip.show();
}

void fan_control_task(void *pvParameters) {
    pinMode(FAN_PIN, OUTPUT);

    fanStrip.begin();
    fanStrip.clear();
    fanStrip.show();

    int lastState = -1;

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        int currentState;

        if (glob_fan_cmd == 1) {
            currentState = 1;
        } else {
            if (glob_temperature > TEMP_THRESHOLD) {
                currentState = 1;
            } else {
                currentState = 0;
            }
        }

        if (currentState != lastState) {
            if (currentState == 1) {
                fan_on();
                // fan_led_on();
            } else {
                fan_off();
                // fan_led_off();
            }

            lastState = currentState;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
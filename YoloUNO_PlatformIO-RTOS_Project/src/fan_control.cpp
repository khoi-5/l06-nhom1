#include "fan_control.h"

extern volatile int glob_fan_cmd;

int miniFanPin = 6;


void fan_control_task(void *pvParameters) {
    pinMode(miniFanPin, OUTPUT);

    while (1) {
        int speed = (glob_fan_cmd == 1) ? 70 : 0;  // 1 -> 70%, 0 -> 0%
        int pwm = map(speed, 0, 100, 0, 255);

        analogWrite(miniFanPin, pwm);

        // Serial.print("Speed: ");
        // Serial.print(speed);
        // Serial.println("%");

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}


// #include "fan_control.h"
// #include <Adafruit_NeoPixel.h>

// #define FAN_LED_PIN 6
// #define FAN_LED_COUNT 4

// extern volatile int glob_fan_cmd;

// Adafruit_NeoPixel fanStrip(FAN_LED_COUNT, FAN_LED_PIN, NEO_GRB + NEO_KHZ800);

// void fan_control_task(void *pvParameters) {
//     fanStrip.begin();
//     fanStrip.clear();
//     fanStrip.show();

//     int lastState = -1;

//     while (1) {
//         int cmd = glob_fan_cmd;

//         if (cmd != lastState) {
//             if (cmd == 0) {
//                 fanStrip.clear();
//                 fanStrip.show();
//                 Serial.println("FAN OFF -> LED OFF");
//             } else {
//                 for (int i = 0; i < FAN_LED_COUNT; i++) {
//                     fanStrip.setPixelColor(i, fanStrip.Color(255, 0, 0));
//                 }
//                 fanStrip.show();
//                 Serial.println("FAN ON -> LED RED");
//             }
//             lastState = cmd;
//         }

//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }
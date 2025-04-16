#include <Arduino.h>
#include "pins.h"
#include "swrleds.h"

void initSWRLEDs(){
    pinMode(SWR_LED_0,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_1,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_2,      OUTPUT_OPEN_DRAIN); 
    pinMode(SWR_LED_3,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_4,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_5,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_6,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_7,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_8,      OUTPUT_OPEN_DRAIN);
    pinMode(SWR_LED_9,      OUTPUT_OPEN_DRAIN);
}

void setSWRLEDs(uint8_t value){
    if (value == 0) {
        digitalWrite(SWR_LED_0, HIGH);
        digitalWrite(SWR_LED_1, HIGH);
        digitalWrite(SWR_LED_2, HIGH);
        digitalWrite(SWR_LED_3, HIGH);
        digitalWrite(SWR_LED_4, HIGH);
        digitalWrite(SWR_LED_5, HIGH);
        digitalWrite(SWR_LED_6, HIGH);
        digitalWrite(SWR_LED_7, HIGH);
        digitalWrite(SWR_LED_8, HIGH);
        digitalWrite(SWR_LED_9, HIGH);
    } 
    if (value >= 1) {
        digitalWrite(SWR_LED_0, LOW);
    }
    if (value >= 2) {
        digitalWrite(SWR_LED_1, LOW);
    }
    if (value >= 3) {
        digitalWrite(SWR_LED_2, LOW);
    }
    if (value >= 4) {
        digitalWrite(SWR_LED_3, LOW);
    }
    if (value >= 5) {
        digitalWrite(SWR_LED_4, LOW);
    }
    if (value >= 6) {
        digitalWrite(SWR_LED_5, LOW);
    }
    if (value >= 7) {
        digitalWrite(SWR_LED_6, LOW);
    }
    if (value >= 8) {
        digitalWrite(SWR_LED_7, LOW);
    }
    if (value >= 9) {
        digitalWrite(SWR_LED_8, LOW);
    }
    if (value >= 10) {
        digitalWrite(SWR_LED_9, LOW);
    }
}


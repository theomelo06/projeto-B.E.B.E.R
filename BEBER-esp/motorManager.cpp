#include "motorManager.hpp"

const int dataPin = 23;
const int clockPin = 18;
const int latchPin = 19;

void configHCPins(){

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
}
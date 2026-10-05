#pragma once

#include<Arduino.h>
#include <ArduinoJson.h>
#include "wifiManager.hpp"
#include "buttonManager.hpp"

void configRequestLEDs();

void receiveData();
void piscarLEDVermelho();
void acenderLEDVerde();
void apagarLEDVerde();
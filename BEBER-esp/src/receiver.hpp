#pragma once

#include<Arduino.h>
#include <ArduinoJson.h>
#include "wifiManager.hpp"
#include "buttonManager.hpp"

inline constexpr int pinoVermelho = 15;
inline constexpr int pinoVerde = 2;

void configRequestLEDs();
void receiveData();
void piscarLEDVermelho();
void acenderLEDVerde();
void apagarLEDVerde();
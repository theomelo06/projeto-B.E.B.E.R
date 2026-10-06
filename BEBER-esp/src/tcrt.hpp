#pragma once

#include <Arduino.h>

inline constexpr int tcrta0 = 34;
inline constexpr int tcrtd0 = 35;

void configTCRT();
bool listenTCRT();
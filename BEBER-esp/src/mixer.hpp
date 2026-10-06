#pragma once

#include <Arduino.h>

inline constexpr int pinoMixer = 12; // deve estar em LOW no boot, então deve ser usado um relé active high

void configMixer();
void ligarMixer();
void desligarMixer();
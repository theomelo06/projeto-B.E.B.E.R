#pragma once

#include <Arduino.h>

// pinos
inline constexpr int dataPin = 23;
inline constexpr int clockPin = 18;
inline constexpr int latchPin = 19;

extern const uint8_t stepSequence[8];

void configHCPins();
void atualizarSaidas(uint32_t mapaSaidas);
void girarMotor(uint8_t motorIndex, float graus);
int grausParaPassos(float graus);
#pragma once

#include <Arduino.h>

inline constexpr int pinoBomba = 0; // deve estar em HIGH no boot, então deve ser usado um relé active low

void configBomba();
void ligarBomba(int tempoEmSegundos);
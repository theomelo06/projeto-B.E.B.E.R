#pragma once

#include <Arduino.h>

inline constexpr int pinoBomba = 3; // TIRAR A CONEXÃO QUANDO FOR PASSAR O CÓDIGO PRA PLACA

void configBomba();
void ligarBomba(int tempoEmSegundos);
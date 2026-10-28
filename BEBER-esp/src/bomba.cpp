#include "bomba.hpp"
#include <Arduino.h>

void configBomba() {
  pinMode(pinoBomba, OUTPUT);
  digitalWrite(pinoBomba, LOW); 
}

void ligarBomba(float tempoEmSegundos) {
  digitalWrite(pinoBomba, HIGH);
  delay(tempoEmSegundos * 1000);
  digitalWrite(pinoBomba, LOW);
}

float mlParaSegundos(int ml) {
  return ml * 0.04; // considerando vazão da bomba de 25ml por segundo
}
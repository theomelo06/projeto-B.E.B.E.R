#include "bomba.hpp"

void configBomba() {
  pinMode(pinoBomba, OUTPUT);
  digitalWrite(pinoBomba, LOW); 
}

void ligarBomba(int tempoEmSegundos) {
  digitalWrite(pinoBomba, HIGH);
  delay(tempoEmSegundos * 1000);
  digitalWrite(pinoBomba, LOW);
}
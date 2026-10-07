#include "bomba.hpp"

void configBomba() {
  pinMode(pinoBomba, OUTPUT);
  digitalWrite(pinoBomba, HIGH); // deve estar em HIGH no boot, então deve ser usado um relé active low
}

void ligarBomba(int tempoEmSegundos) {
  digitalWrite(pinoBomba, LOW); // ativa a bomba (relé active low)
  delay(tempoEmSegundos * 1000);
  digitalWrite(pinoBomba, HIGH);
}
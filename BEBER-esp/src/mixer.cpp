#include "mixer.hpp"

void configMixer() {
  pinMode(pinoMixer, OUTPUT);
  digitalWrite(pinoMixer, LOW);// deve estar em LOW no boot, então deve ser usado um relé active high
}

void ligarMixer() {
  digitalWrite(pinoMixer, HIGH);
}

void desligarMixer() {
  digitalWrite(pinoMixer, LOW);
}
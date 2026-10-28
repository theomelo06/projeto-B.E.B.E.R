#include "tcrt.hpp"
#include <Arduino.h>


void configTCRT(){
  pinMode(tcrtd0, INPUT);
  pinMode(tcrta0, INPUT);
}

// TODO: testar
bool listenTCRT(){
  int distancia = map(analogRead(tcrta0), 0, 1023, 0, 255); // ver se vamos usar
  unsigned long tempoInicio = millis();

  while (millis() - tempoInicio < 5000) {
    if (digitalRead(tcrtd0) == HIGH) {
      return false;
    }
    yield(); // Evita que o Watchdog reinicie a ESP32
  }

  return true;
}
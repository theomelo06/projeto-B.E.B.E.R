#include "tcrt.hpp"
#include <Arduino.h>


void configTCRT(){
  pinMode(tcrtd0, INPUT);
  pinMode(ledPresenca, OUTPUT);
  digitalWrite(ledPresenca, LOW);
}

void isPresent() {
  unsigned long tempoInicio = 0;
  bool contando = false;

  while (true) {
    if (digitalRead(tcrtd0) == LOW) {
      // Se não estava contando, marca o início do tempo
      if (!contando) {
        tempoInicio = millis();
        contando = true;
        acenderLEDPresenca();
      } 
      // Se já estava contando, verifica se passaram 3000 ms (3 segundos)
      else if (millis() - tempoInicio >= 3000) {
        apagarLEDPresenca();
        break; 
      }
    } 
    else {
      // Se o pino for HIGH em qualquer momento, zera a contagem
      apagarLEDPresenca();
      contando = false;
    }
    delay(10); 
  }
}

void acenderLEDPresenca(){

    digitalWrite(ledPresenca, HIGH);
}

void apagarLEDPresenca(){

    digitalWrite(ledPresenca, LOW);
}
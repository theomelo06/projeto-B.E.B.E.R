#include "tcrt.hpp"


void configTCRT(){
  pinMode(tcrtd0, INPUT);
  pinMode(tcrta0, INPUT);
}

// TODO: testar
bool listenTCRT(){

  int distancia = map(analogRead(tcrta0), 0, 1023, 0, 255); // TODO: testar e ver se vamos usar
  unsigned long tempoInicio = millis();

  // Executa o laço por 5000 milissegundos
  while (millis() - tempoInicio < 5000) {
    
    if (digitalRead(tcrtd0) == HIGH) {return false;}
  }

  // não leu HIGH, objeto perto do sensor por 5s
  return true;
}
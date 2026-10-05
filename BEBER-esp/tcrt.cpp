#include "tcrt.hpp"

const int tcrta0 = 34;
const int tcrtd0 = 35;

void configTCRT(){

  pinMode(tcrtd0, INPUT);
  pinMode(tcrta0, INPUT);
}

// TODO: deixar certo essa porra
void listenTCRT(){

  int distanciatcrt = analogRead(tcrta0);
  int presencaTcrt = digitalRead(trctd0);
  
  if (presencaTcrt == LOW){
    Serial.print("Objeto detectado: %d", distanciaTcrt);
  }
  else{
    Serial.print("Nenhuma deteccao");
  }
}
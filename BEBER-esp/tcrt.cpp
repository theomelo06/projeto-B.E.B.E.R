#include "tcrt.hpp"

tcrta0 = 34;
tcrtd0 = 32;

void configTCRT(){

  pinMode(tcrtd0, INPUT);
}


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
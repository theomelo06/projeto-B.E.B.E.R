#include "tcrt.hpp"
#include <Arduino.h>


void configTCRT(){
  pinMode(tcrtd0, INPUT);
}

bool listenTCRT(){

  while (digitalRead(tcrtd0) == HIGH){
    delay(50);
  }

  int counter = 0;
  while (digitalRead(tcrtd0) == LOW){
    counter++;
    delay(50);
    if (counter == 20){
      return true;
    }
  }
  return false;
}
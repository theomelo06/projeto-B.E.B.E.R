#include "buttonManager.hpp"
#include "tcrt.hpp"
#include "receiver.hpp"
#include "wifiManager.hpp"
#include "motorManager.hpp"
#include "mixer.hpp"
#include "bomba.hpp"

inline constexpr int ledOnOff = 5;

void setup() {

  Serial.begin(115200);
  configBomba();
  configMixer();
  configRequestLEDs();
  configHCPins();
  configuraBotoes();
  configTCRT();
  setupWifi();
  pinMode(ledOnOff, OUTPUT);
  digitalWrite(ledOnOff, HIGH); // liga o ledOnOff
  
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TIRAR TODOS OS SERIAL PRINTS DOS ARQUIVOS ANTES DE SUBIR O CÓDIGO PARA O ESP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void loop() {

  processarWifi();
  receiveData();
}


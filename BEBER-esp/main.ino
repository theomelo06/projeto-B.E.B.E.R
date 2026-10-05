#include "buttonManager.hpp"
#include "tcrt.hpp"
#include "receiver.hpp"
#include "wifiManager.hpp"

const int ledOnOff = 33;

void setup() {

  Serial.begin(115200);

  // chama as funções de configurações:
  pinMode(ledOnOff, OUTPUT);
  configRequestLEDs();
  // do buttonManager
  // do tcrt
  // do receiver
  // do setupWifi

}

void loop() {

}

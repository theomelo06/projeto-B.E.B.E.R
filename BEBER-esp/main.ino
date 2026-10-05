#include "buttonManager.hpp"
#include "tcrt.hpp"
#include "receiver.hpp"
#include "wifiManager.hpp"
#include "motorManager.hpp"

const int ledOnOff = 5;

void setup() {

  Serial.begin(115200);

  pinMode(ledOnOff, OUTPUT);
  digitalWrite(ledOnOff, HIGH); // liga o ledOnOff
  configRequestLEDs();
  configHCPins();
  configuraBotoes();
  configTCRT();
  setupWifi();

}

void loop() {

  // TODO: morte
}

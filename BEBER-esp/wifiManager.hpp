#pragma once

#include<Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// TODO: configurar essa bosta
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* apiURL = "http://your-api-url.com";

void setupWifi();
String receiveWifiRequest();

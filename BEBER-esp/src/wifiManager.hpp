#pragma once

#include<Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// TODO: configurar essa bosta
inline constexpr const char* ssid = "Wokwi-GUEST";
inline constexpr const char* password = "";
inline constexpr const char* apiURL = "http://your-api-url.com";

void setupWifi();
String receiveWifiRequest();

#pragma once

#include<Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid;
const char* password;
const char* apiURL;

void setupWifi();
String receiveWifiRequest();

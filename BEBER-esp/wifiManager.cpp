#include "wifiManager.hpp"

ssid = "Wokwi-GUEST";
password = "";
apiURL = "http://your-api-url.com";

// configura o wifi, chamado no setup()
void setupWifi(){

    Wifi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado com o IP: " + WiFi.localIP().toString());
}

// Ou recebe o json do site ou o sinal do botão, dependendo do que for mais recente.
String receiveWifiRequest(){

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(apiURL);
        int httpResponseCode = http.GET();

        if (httpResponseCode == 200) {
            String payload = http.getString();
            return payload;
        } else {
            Serial.println("Erro na requisição HTTP: " + String(httpResponseCode));
            return "";
        }
        http.end();
    } else {
        Serial.println("WiFi não conectado");
    }
    return "";
}
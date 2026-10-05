#include "receiver.hpp"

const int pinoVermelho = 15;
const int pinoVerde = 2;

void configRequestLEDs(){

    pinMode(pinoVermelho, OUTPUT);
    pinMode(pinoVerde, OUTPUT);
}

void receiveData(){

    String wifiRequest = receiveWifiRequest();
    int signalRequest = receiveButtonsRequest();

    if (request != ""){

        // Há um pedido via wifi
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, wifiRequest);

        if (error) {
          Serial.print("Falha ao interpretar o JSON: ");
          Serial.println(error.c_str());
          piscarLEDVermelho();
          return;
        }

        int WheyGramas = doc["WheyGramas"];
        int AguaMl = doc["AguaMl"];
        int LeiteNinhoGramas = doc["LeiteNinhoGramas"];
        int Sabor = doc["Sabor"];
        
        if (Sabor == 1){
            // Sabor Chocolate
        }
        else if (Sabor == 2){
            // Sabor Morango
        }
        else if (Sabor == 3){
            // Sabor Baunilha
        }
        else if (Sabor == 4){
            // Sabor Cookies
        }
        else if (Sabor == 5){
            // Sabor MorangoChoco
        }
        else if (Sabor == 6){
            // Sabor 6
        }
        
    }
    else if (signalRequest != -1){
        
        // Há um pedido pelos botões

        if (signalRequest == 10){

            // predefinição 1
        }
        else if (signalRequest == 20){

            // predefinição 2
        }
        else if (signalRequest == 30){

            // predefinição 3
        }
        else if (signalRequest == 40){

            // predefinição 4
        }
        else if (signalRequest == 50){

            // predefinição 5
        }
        else if (signalRequest == 60){

            // predefinição 6
        }
    }
}


void piscarLEDVermelho() {

    digitalWrite(pinoVermelho, HIGH); // vai travar e fds
    delay(500);
    digitalWrite(pinoVermelho, LOW);
    delay(500);
    digitalWrite(pinoVermelho, HIGH);
    delay(500);
    digitalWrite(pinoVermelho, LOW); 
}

void acenderLEDVerde(){

    digitalWrite(pinoVerde, HIGH);
}

void apagarLEDVerde(){

    digitalWrite(pinoVerde, LOW);
}

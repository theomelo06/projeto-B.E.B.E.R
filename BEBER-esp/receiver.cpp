#include "receiver.hpp"

pinoVermelho = 23;
pinoVerde = 22;

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


void piscarLEDVermelho(){

    // TODO
}

void acenderLEDVerde(){

    // TODO
}

void apagarLEDVerde(){

    // TODO
}

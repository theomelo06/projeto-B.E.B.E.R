#include "receiver.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifiManager.hpp"
#include "buttonManager.hpp"
#include "motorManager.hpp"
#include "mixer.hpp"
#include "bomba.hpp"
#include "tcrt.hpp"


void configRequestLEDs(){

    pinMode(pinoVermelho, OUTPUT);
    pinMode(pinoVerde, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
    pinMode(led5, OUTPUT);
    pinMode(led6, OUTPUT);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
    digitalWrite(led6, LOW);
    digitalWrite(pinoVermelho, LOW);
    digitalWrite(pinoVerde, LOW);
}

void receiveData(){

    String wifiRequest = receiveWifiRequest();
    int signalRequest = receiveButtonRequest();

    if (wifiRequest != ""){

        // Há um pedido via wifi

        // Acende o LED verde para inidicar que o pedido está em preparo
        acenderLEDVerde();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, wifiRequest);

        if (error) {
          Serial.print("Falha ao interpretar o JSON: ");
          Serial.println(error.c_str());
          apagarLEDVerde();
          piscarLEDVermelho(); // pisca para indicar que houve erro no processamento, pedido cancelado
          return;
        }

        int WheyGramas = doc["wheyGramas"].as<int>();
        int AguaMl = doc["aguaMl"].as<int>();
        int LeiteNinhoGramas = doc["leiteNinhoGramas"].as<int>();
        int Sabor = doc["sabor"].as<int>();
                
        Serial.println("\n--- Pedido recebido por Wi-Fi ---");

        Serial.print("ID: ");
        Serial.println(doc["pedidoId"].as<String>());

        Serial.print("Whey: ");
        Serial.print(WheyGramas);
        Serial.println(" g");

        Serial.print("Agua: ");
        Serial.print(AguaMl);
        Serial.println(" mL");

        Serial.print("Leite Ninho: ");
        Serial.print(LeiteNinhoGramas);
        Serial.println(" g");

        Serial.print("Sabor: ");
        Serial.println(Sabor);

        Serial.println("--------------------------------");
        
        // espera o copo estar posicionado corretamente, usando o sensor TCRT
        while (listenTCRT() == false) {
            delay(1);
        }

        // ligar e desligar bomba pela qtde solicitada
        ligarBomba(mlParaSegundos(AguaMl));

        // ligar mixer
        ligarMixer();

        // despejar ninho se tiver
        if (LeiteNinhoGramas > 0){
            girarMotor(INDEX_NINHO, gramasParaGraus(LeiteNinhoGramas));
        }

        // espera terminar de mexer o ninho
        delay(TEMPO_ESPERA);

        // despejar whey

        if (Sabor == 1){
            girarMotor(INDEX_CHOCOLATE, gramasParaGraus(WheyGramas)); // Sabor Chocolate
        }
        else if (Sabor == 2){
            girarMotor(INDEX_MORANGO, gramasParaGraus(WheyGramas)); // Sabor Morango
        }
        else if (Sabor == 3){
            girarMotor(INDEX_BAUNILHA, gramasParaGraus(WheyGramas)); // Sabor Baunilha
        }
        else if (Sabor == 4){
            girarMotor(INDEX_COOKIES, gramasParaGraus(WheyGramas)); // Sabor Cookies
        }
        else if (Sabor == 5){
            girarMotor(INDEX_MORANGOCHOCO, gramasParaGraus(WheyGramas)); // Sabor MorangoChoco
        }

        // espera terminar de mexer o whey
        delay(TEMPO_ESPERA);

        // desligar mixer
        deligarMixer();

        apagarLEDVerde(); // Apaga o LED verde ao fim do preparo
        
    }
    else if (signalRequest != -1){
        
        // Há um pedido pelos botões

        // Acende o LED verde para inidicar que o pedido está em preparo
        acenderLEDVerde();

        if (signalRequest == 10){

            digitalWrite(led1, HIGH); // Acende o LED do botão 1

            // PREDEFINIÇÃO 1 - SMOOTH CHOCOLATE
            /*  300ml agua
            *   25g ninho
            *   30g whey chocolate
            */

            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(300));
            ligarMixer();
            girarMotor(INDEX_NINHO, gramasParaGraus(25));
            delay(TEMPO_ESPERA);
            girarMotor(INDEX_CHOCOLATE, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led1, LOW); // Apaga o LED do botão 1 ao fim do preparo
        }
        else if (signalRequest == 20){

            digitalWrite(led2, HIGH); // Acende o LED do botão 2

            // PREDEFINIÇÃO 2 - SMOOTH STRAWBERRY
            /*  300ml agua
            *   25g ninho
            *   30g whey morango
            */
            
            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(300));
            ligarMixer();
            girarMotor(INDEX_NINHO, gramasParaGraus(25));
            delay(TEMPO_ESPERA);
            girarMotor(INDEX_MORANGO, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led2, LOW); // Apaga o LED do botão 2 ao fim do preparo
        }
        else if (signalRequest == 30){

            digitalWrite(led3, HIGH); // Acende o LED do botão 3

            // PREDEFINIÇÃO 3 - SMOOTH VANILLA
            /*  300ml agua
            *   25g ninho
            *   30g whey baunilha
            */

            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(300));
            ligarMixer();
            girarMotor(INDEX_NINHO, gramasParaGraus(25));
            delay(TEMPO_ESPERA);
            girarMotor(INDEX_BAUNILHA, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led3, LOW); // Apaga o LED do botão 3 ao fim do preparo
        }
        else if (signalRequest == 40){

            digitalWrite(led4, HIGH); // Acende o LED do botão 4

            // PREDEFINIÇÃO 4 - SMOOTH COOKIES
            /*  300ml agua
            *   25g ninho
            *   30g whey cookies
            */

            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(300));
            ligarMixer();
            girarMotor(INDEX_NINHO, gramasParaGraus(25));
            delay(TEMPO_ESPERA);
            girarMotor(INDEX_COOKIES, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led4, LOW); // Apaga o LED do botão 4 ao fim do preparo
        }
        else if (signalRequest == 50){

            digitalWrite(led5, HIGH); // Acende o LED do botão 5

            // PREDEFINIÇÃO 5 - SWEET MIX
            /*  300ml agua
            *   25g ninho
            *   30g whey morangochoco
            */

            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(300));
            ligarMixer();
            girarMotor(INDEX_NINHO, gramasParaGraus(25));
            delay(TEMPO_ESPERA);
            girarMotor(INDEX_MORANGOCHOCO, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led5, LOW); // Apaga o LED do botão 5 ao fim do preparo
        }
        else if (signalRequest == 60){

            digitalWrite(led6, HIGH); // Acende o LED do botão 6

            // PREDEFINIÇÃO 6 - POWER DOSE
            /*  220ml agua
            *   30g whey baunilha
            */

            while (listenTCRT() == false) {delay(1);}
            ligarBomba(mlParaSegundos(220));
            ligarMixer();
            girarMotor(INDEX_BAUNILHA, gramasParaGraus(30));
            delay(TEMPO_ESPERA);
            desligarMixer();

            digitalWrite(led6, LOW); // Apaga o LED do botão 6 ao fim do preparo
        }

        apagarLEDVerde(); // Apaga o LED verde ao fim do preparo
    }
}


void piscarLEDVermelho() {

    digitalWrite(pinoVermelho, HIGH); // vai travar e fds
    delay(200);
    digitalWrite(pinoVermelho, LOW);
    delay(200);
    digitalWrite(pinoVermelho, HIGH);
    delay(200);
    digitalWrite(pinoVermelho, LOW); 
    delay(200);
    digitalWrite(pinoVermelho, HIGH);
    delay(200);
    digitalWrite(pinoVermelho, LOW);
}

void acenderLEDVerde(){

    digitalWrite(pinoVerde, HIGH);
}

void apagarLEDVerde(){

    digitalWrite(pinoVerde, LOW);
}

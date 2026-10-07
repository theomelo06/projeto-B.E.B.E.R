#include "wifiManager.hpp"
#include "wifiConfig.hpp"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

namespace {
    WebServer servidor(80);

    String pedidoPendente;
    bool estavaConectado = false;
    bool servidorIniciado = false;
    unsigned long ultimaTentativa = 0;

    void responderErro(int codigo, const char* mensagem) {
        JsonDocument resposta;
        resposta["mensagem"] = mensagem;

        String json;
        serializeJson(resposta, json);
        servidor.send(codigo, "application/json", json);
    }

    void receberPedidoHttp() {
        String corpo = servidor.arg("plain");

        if (corpo.isEmpty() || corpo.length() > 1024) {
            responderErro(400, "Corpo do pedido vazio ou muito grande.");
            return;
        }

        JsonDocument pedido;

        if (deserializeJson(pedido, corpo)) {
            responderErro(400, "JSON invalido.");
            return;
        }

        if (!pedido["pedidoId"].is<const char*>() ||
            !pedido["wheyGramas"].is<int>() ||
            !pedido["aguaMl"].is<int>() ||
            !pedido["leiteNinhoGramas"].is<int>() ||
            !pedido["sabor"].is<int>()) {
            responderErro(400, "Campos ausentes ou tipos invalidos.");
            return;
        }

        String pedidoId = pedido["pedidoId"].as<String>();

        int whey = pedido["wheyGramas"].as<int>();
        int agua = pedido["aguaMl"].as<int>();
        int leite = pedido["leiteNinhoGramas"].as<int>();
        int sabor = pedido["sabor"].as<int>();

        if (pedidoId.length() != 36) {
            responderErro(400, "Identificador de pedido invalido.");
            return;
        }

        if (agua < 100 || agua > 500) {
            responderErro(400, "A agua deve estar entre 100 e 500 mL.");
            return;
        }

        if (whey < 0 || whey > 30 || whey > agua / 10) {
            responderErro(
                400,
                "Whey invalido: maximo de 30 g e 10 g por 100 mL."
            );
            return;
        }

        if (leite < 0 || leite > agua / 5) {
            responderErro(
                400,
                "Leite Ninho invalido: maximo de 20 g por 100 mL."
            );
            return;
        }

        if (sabor < 1 || sabor > 5) {
            responderErro(400, "Escolha um dos cinco sabores.");
            return;
        }

        if (!pedidoPendente.isEmpty()) {
            responderErro(409, "Existe um pedido aguardando leitura.");
            return;
        }

        // Guarda o JSON para o receiver.cpp consumir no loop.
        pedidoPendente = corpo;

        JsonDocument resposta;
        resposta["pedidoId"] = pedidoId;
        resposta["estado"] = "recebido";
        resposta["mensagem"] = "Pedido recebido pela ESP32.";

        String json;
        serializeJson(resposta, json);

        // Confirma recebimento. Nenhum atuador foi acionado.
        servidor.send(200, "application/json", json);
    }
}

void setupWifi() {
    servidor.on("/api/status", HTTP_GET, []() {
        servidor.send(
            200,
            "application/json",
            "{\"estado\":\"online\",\"modo\":\"teste-comunicacao\"}"
        );
    });

    servidor.on("/api/pedidos", HTTP_POST, receberPedidoHttp);

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    ultimaTentativa = millis();
    Serial.println("Conectando ao Wi-Fi...");
}

void processarWifi() {
    bool conectado = WiFi.status() == WL_CONNECTED;

    if (conectado) {
        if (!estavaConectado) {
            Serial.print("ESP conectada. IP: ");
            Serial.println(WiFi.localIP());
        }

        if (!servidorIniciado) {
            servidor.begin();
            servidorIniciado = true;
        }

        servidor.handleClient();
    } else {
        if (estavaConectado) {
            Serial.println("Wi-Fi desconectado.");
        }

        if (millis() - ultimaTentativa >= 15000) {
            ultimaTentativa = millis();
            Serial.println("Tentando reconectar ao Wi-Fi...");
            WiFi.reconnect();
        }
    }

    estavaConectado = conectado;
}

String receiveWifiRequest() {
    String pedido = pedidoPendente;
    pedidoPendente = "";
    return pedido;
}
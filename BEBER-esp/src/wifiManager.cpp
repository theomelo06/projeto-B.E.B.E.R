#include "wifiManager.hpp"
#include "wifiConfig.hpp"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

namespace {
    WebServer servidor(80);

    SemaphoreHandle_t trava = nullptr;
    TaskHandle_t tarefa = nullptr;

    String pedidoPendente;
    String ultimoCorpo;
    String pedidoId;
    String origem;

    String estado = "ocioso";
    String mensagem = "Máquina disponível.";

    bool ocupada = false;
    unsigned long contadorLocal = 0;

    // Protege os dados compartilhados entre rede e preparo.
    struct Guarda {
        Guarda() {
            xSemaphoreTake(trava, portMAX_DELAY);
        }

        ~Guarda() {
            xSemaphoreGive(trava);
        }
    };

    String statusJson() {
        JsonDocument doc;

        {
            Guarda guarda;

            doc["pedidoId"] = pedidoId;
            doc["origem"] = origem;
            doc["ocupada"] = ocupada;
            doc["estado"] = estado;
            doc["mensagem"] = mensagem;
        }

        String json;
        serializeJson(doc, json);

        return json;
    }

    void responderErro(int codigo, const char* texto) {
        JsonDocument doc;
        doc["mensagem"] = texto;

        String json;
        serializeJson(doc, json);

        servidor.send(codigo, "application/json", json);
    }

    void receberPedidoHttp() {
        String corpo = servidor.arg("plain");
        JsonDocument pedido;

        if (
            corpo.isEmpty() ||
            corpo.length() > 1024 ||
            deserializeJson(pedido, corpo)
        ) {
            responderErro(
                400,
                "Corpo vazio, grande demais ou JSON inválido."
            );
            return;
        }

        if (!pedido["pedidoId"].is<const char*>()) {
            responderErro(400, "Identificador ausente.");
            return;
        }

        String novoId = pedido["pedidoId"].as<String>();

        if (novoId.length() != 36) {
            responderErro(400, "Identificador inválido.");
            return;
        }

        JsonDocument normalizado;
        normalizado["pedidoId"] = novoId;

        if (!pedido["predefinicao"].isNull()) {
            if (!pedido["predefinicao"].is<int>()) {
                responderErro(400, "Predefinição inválida.");
                return;
            }

            int predefinicao = pedido["predefinicao"];

            if (predefinicao < 1 || predefinicao > 6) {
                responderErro(
                    400,
                    "Escolha uma das seis predefinições."
                );
                return;
            }

            normalizado["predefinicao"] = predefinicao;
        } else {
            if (
                !pedido["wheyGramas"].is<int>() ||
                !pedido["aguaMl"].is<int>() ||
                !pedido["leiteNinhoGramas"].is<int>() ||
                !pedido["sabor"].is<int>()
            ) {
                responderErro(
                    400,
                    "Campos ausentes ou tipos inválidos."
                );
                return;
            }

            int whey = pedido["wheyGramas"];
            int agua = pedido["aguaMl"];
            int leite = pedido["leiteNinhoGramas"];
            int sabor = pedido["sabor"];

            if (
                agua < 100 || agua > 500 ||
                whey < 0 || whey > 30 ||
                whey > agua / 10 ||
                leite < 0 || leite > agua / 5 ||
                sabor < 1 || sabor > 5
            ) {
                responderErro(
                    400,
                    "Receita personalizada fora dos limites."
                );
                return;
            }

            normalizado["wheyGramas"] = whey;
            normalizado["aguaMl"] = agua;
            normalizado["leiteNinhoGramas"] = leite;
            normalizado["sabor"] = sabor;
        }

        String canonico;
        serializeJson(normalizado, canonico);

        bool repetido = false;
        bool conflito = false;

        {
            Guarda guarda;

            if (novoId == pedidoId) {
                repetido = canonico == ultimoCorpo;
                conflito = !repetido;
            } else if (ocupada) {
                conflito = true;
            } else {
                ocupada = true;
                pedidoId = novoId;
                origem = "wifi";

                estado = "recebido";
                mensagem = "Pedido recebido pela ESP32.";

                ultimoCorpo = canonico;
                pedidoPendente = canonico;
            }
        }

        if (conflito) {
            responderErro(
                409,
                "Máquina ocupada ou identificador já utilizado com outra receita."
            );
            return;
        }

        servidor.send(
            repetido ? 200 : 202,
            "application/json",
            statusJson()
        );
    }

    void atenderRede(void*) {
        bool iniciado = false;
        bool conectadoAntes = false;

        for (;;) {
            bool conectado = WiFi.status() == WL_CONNECTED;

            if (conectado) {
                if (!conectadoAntes) {
                    Serial.print("ESP conectada. IP: ");
                    Serial.println(WiFi.localIP());
                }

                if (!iniciado) {
                    servidor.begin();
                    iniciado = true;
                }

                servidor.handleClient();
            }

            conectadoAntes = conectado;

            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void setupWifi() {
    trava = xSemaphoreCreateMutex();

    if (!trava) {
        Serial.println("Falha ao criar mutex HTTP.");
        return;
    }

    servidor.on("/api/status", HTTP_GET, []() {
        servidor.send(
            200,
            "application/json",
            statusJson()
        );
    });

    servidor.on(
        "/api/pedidos",
        HTTP_POST,
        receberPedidoHttp
    );

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    if (
        xTaskCreate(
            atenderRede,
            "BEBER-HTTP",
            8192,
            nullptr,
            1,
            &tarefa
        ) != pdPASS
    ) {
        Serial.println("Falha ao criar tarefa HTTP.");
    }
}

// Mantida para preservar o loop atual do main.ino.
// O atendimento HTTP passou para a tarefa atenderRede.
void processarWifi() {
}

String receiveWifiRequest() {
    if (!trava) return "";

    Guarda guarda;

    String pedido = pedidoPendente;
    pedidoPendente = "";

    if (!pedido.isEmpty()) {
        estado = "preparando";

        mensagem =
            "Pedido em execução. Posicione o copo; "
            "a máquina pode estar aguardando sua presença.";
    }

    return pedido;
}

bool reservarPedidoLocal(int sinal) {
    if (!trava) return false;

    Guarda guarda;

    if (ocupada) return false;

    ocupada = true;

    pedidoId =
        "botao-" +
        String(++contadorLocal) +
        "-" +
        String(sinal);

    origem = "botao";
    ultimoCorpo = "";

    estado = "preparando";
    mensagem = "Pedido iniciado pelo botão. Posicione o copo.";

    return true;
}

void finalizarPedido(bool sucesso) {
    if (!trava) return;

    Guarda guarda;

    ocupada = false;
    estado = sucesso ? "finalizado" : "erro";

    mensagem = sucesso
        ? "Rotina de preparo concluída."
        : "Falha ao interpretar o pedido.";
}
#pragma once

#include <Arduino.h>

constexpr int NUM_BOTOES = 6;
constexpr unsigned long TEMPO_VALIDACAO = 1000; // 1 segundo

// Definição da estrutura
struct Botao {
  int pinoBotao;
  int pinoLed;
  int sinal;
  unsigned long tempoInicio;
  bool estaPressionado;
  bool sinalJaProcessado;
};

extern Botao botoesPedido[NUM_BOTOES];

// Protótipos das funções
void configuraBotoes();
void apagarTodosLeds();
int receiveButtonRequest();
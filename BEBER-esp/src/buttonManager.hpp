#pragma once

#include <Arduino.h>

inline constexpr int NUM_BOTOES = 6;
inline constexpr unsigned long TEMPO_VALIDACAO = 1000; // 1 segundo

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

// funções
void configuraBotoes();
void apagarTodosLeds();
int receiveButtonRequest();
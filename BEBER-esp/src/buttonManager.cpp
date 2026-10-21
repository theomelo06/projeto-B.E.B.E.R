#include "buttonManager.hpp"
#include <Arduino.h>

// inicializa os botões
Botao botoesPedido[NUM_BOTOES] = {
  {14, 10, 0, false, false}, // Botão 1, Sinal 10
  {25, 20, 0, false, false}, // Botão 2, Sinal 20
  {26, 30, 0, false, false}, // Botão 3, Sinal 30
  {27, 40, 0, false, false}, // Botão 4, Sinal 40
  {32, 50, 0, false, false}, // Botão 5, Sinal 50
  {33, 60, 0, false, false}  // Botão 6, Sinal 60
};

// Configura todos os botões, deve ser chamada no setup()
void configuraBotoes(){
  for (int i = 0; i < NUM_BOTOES; i++) {
    pinMode(botoesPedido[i].pinoBotao, INPUT_PULLUP);
  }
}

// Leitura dos botões
int receiveButtonRequest() {
  
  // Conta quantos botões estão fisicamente pressionados agora
  int qtdPressionados = 0;
  for (int i = 0; i < NUM_BOTOES; i++) {
    if (digitalRead(botoesPedido[i].pinoBotao) == LOW) {
      qtdPressionados++;
    }
  }

  // Se o usuário tentar apertar mais de um botão ao mesmo tempo, aborta tudo
  if (qtdPressionados > 1) {
    for (int i = 0; i < NUM_BOTOES; i++) {
      botoesPedido[i].estaPressionado = false;
      botoesPedido[i].sinalJaProcessado = false;
    }
    return -1; // Sai da função sem validar nenhum sinal
  }

  // Lógica normal de validação de tempo
  for (int i = 0; i < NUM_BOTOES; i++) {
    bool leituraAtual = (digitalRead(botoesPedido[i].pinoBotao) == LOW);
    
    // CASO 1: O botão começou a ser pressionado
    if (leituraAtual == true && botoesPedido[i].estaPressionado == false) {
      botoesPedido[i].estaPressionado = true;
      botoesPedido[i].tempoInicio = millis();
    }
    
    // CASO 2: O botão continua sendo pressionado
    else if (leituraAtual == true && botoesPedido[i].estaPressionado == true) {
      if ((millis() - botoesPedido[i].tempoInicio >= TEMPO_VALIDACAO) && !botoesPedido[i].sinalJaProcessado) {
        botoesPedido[i].sinalJaProcessado = true; 
        return botoesPedido[i].sinal; 
      }
    }
    
    // CASO 3: O botão foi solto
    else if (leituraAtual == false && botoesPedido[i].estaPressionado == true) {
      botoesPedido[i].estaPressionado = false;
      botoesPedido[i].sinalJaProcessado = false;
    }
  }
  
  return -1;
}
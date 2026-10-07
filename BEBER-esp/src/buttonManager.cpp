#include "buttonManager.hpp"

// inicializa os botões
Botao botoesPedido[NUM_BOTOES] = {
  {14, 4, 10, 0, false, false}, // Botão 1, LED 1, Sinal 10
  {25, 13, 20, 0, false, false}, // Botão 2, LED 2, Sinal 20
  {26, 16, 30, 0, false, false}, // Botão 3, LED 3, Sinal 30
  {27, 17,  40, 0, false, false}, // Botão 4, LED 4,  Sinal 40
  {32, 21, 50, 0, false, false}, // Botão 5, LED 5, Sinal 50
  {33, 22, 60, 0, false, false}  // Botão 6, LED 6, Sinal 60
};

// Configura todos os botões, deve ser chamada no setup()
void configuraBotoes(){
  for (int i = 0; i < NUM_BOTOES; i++) {
    pinMode(botoesPedido[i].pinoBotao, INPUT_PULLUP);
    pinMode(botoesPedido[i].pinoLed, OUTPUT);
    digitalWrite(botoesPedido[i].pinoLed, LOW); // Garante que todos os LEDs comecem apagados
  }
}

// Apaga todos os LEDs para garantir que apenas um fique aceso
void apagarTodosLeds() {
  for (int i = 0; i < NUM_BOTOES; i++) {
    digitalWrite(botoesPedido[i].pinoLed, LOW);
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
        
        apagarTodosLeds();
        digitalWrite(botoesPedido[i].pinoLed, HIGH);

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
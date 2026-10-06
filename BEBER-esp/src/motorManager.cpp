#include "motorManager.hpp"


// Sequencia de passos para o 28BYJ-48 (Half-Step - 8 fases)
const uint8_t stepSequence[8] = {
    0b1000, 0b1100, 0b0100, 0b0110,
    0b0010, 0b0011, 0b0001, 0b1001
};


// Funcao para enviar os 24 bits para os 3 CIs em cascata
void atualizarSaidas(uint32_t mapaSaidas) {
    
    digitalWrite(latchPin, LOW);

    // Envia 3 bytes (24 bits total), do ultimo CI (3) para o primeiro CI (1)
    shiftOut(dataPin, clockPin, MSBFIRST, (mapaSaidas >> 16) & 0xFF);
    shiftOut(dataPin, clockPin, MSBFIRST, (mapaSaidas >> 8) & 0xFF);
    shiftOut(dataPin, clockPin, MSBFIRST, mapaSaidas & 0xFF);

    digitalWrite(latchPin, HIGH); // Aplica as mudancas nas saidas
}


// Funcao para girar um motor especifico (motorIndex de 0 a 5)
void girarMotor(uint8_t motorIndex, float graus) {

    int passos = grausParaPassos(graus);
    
    for (int i = 0; i < passos; i++) {
        uint32_t mapa = 0;
        
        // Busca a fase atual (0 a 7) e desloca 4 bits para o motor correto
        mapa |= ((uint32_t)(stepSequence[i % 8] & 0x0F)) << (motorIndex * 4);
        
        atualizarSaidas(mapa);
        
        // Atraso entre os passos (2ms é o ideal para o 28BYJ-48 manter o torque)
        delay(2); 
    }

    // Desliga todas as bobinas após o movimento para evitar aquecimento
    atualizarSaidas(0); 
}


// Converte um ângulo em graus para o número de passos em modo Half-Step
int grausParaPassos(float graus) {
    // Constante de passos por volta exata do 28BYJ-48 (Half-step)
    const float PASSOS_POR_VOLTA = 4075.77; 
    
    // Calcula e arredonda para o número inteiro de passos mais próximo
    int passos = round((graus / 360.0) * PASSOS_POR_VOLTA);
    
    return passos;
}


// Config inicial chamada no setup
void configHCPins(){

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    atualizarSaidas(0x00000000); //desliga as saidas quando liga
}
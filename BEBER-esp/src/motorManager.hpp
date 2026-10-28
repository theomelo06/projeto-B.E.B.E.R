#pragma once

inline constexpr float GRAUS_POR_GRAMA = 1.0; // TESTAR
inline constexpr float PASSOS_POR_VOLTA = 4075.77; // Constante de passos por volta exata do 28BYJ-48 (Half-step)

inline constexpr int dataPin = 23;
inline constexpr int clockPin = 18;
inline constexpr int latchPin = 19;

// Sequencia de passos para o 28BYJ-48 (Half-Step - 8 fases)
inline constexpr uint8_t stepSequence[8] = {
    0b1000, 0b1100, 0b0100, 0b0110,
    0b0010, 0b0011, 0b0001, 0b1001
};

void configHCPins();
void atualizarSaidas(uint32_t mapaSaidas);
void girarMotor(uint8_t motorIndex, float graus);
int grausParaPassos(float graus);
float gramasParaGraus(int gramas);
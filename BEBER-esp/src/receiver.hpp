#pragma once

inline constexpr int TEMPO_ESPERA = 10000; // tempo de espera em milissegundos

inline constexpr int INDEX_CHOCOLATE = 0;
inline constexpr int INDEX_MORANGO = 1;
inline constexpr int INDEX_BAUNILHA = 2;
inline constexpr int INDEX_COOKIES = 3;
inline constexpr int INDEX_MORANGOCHOCO = 4;
inline constexpr int INDEX_NINHO = 5;

inline constexpr int pinoVermelho = 15;
inline constexpr int pinoVerde = 2;

inline constexpr int led1 = 4;
inline constexpr int led2 = 13;
inline constexpr int led3 = 16;
inline constexpr int led4 = 17;
inline constexpr int led5 = 21;
inline constexpr int led6 = 22;

void configRequestLEDs();
void receiveData();
void piscarLEDVermelho();
void acenderLEDVerde();
void apagarLEDVerde();
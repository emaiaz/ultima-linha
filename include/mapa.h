#ifndef MAPA_H
#define MAPA_H

#include "jogo.h"

void inicializarMapa(Jogo *jogo);
void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna);
void criarCaminho(Jogo *jogo);
int ehCaminho(Jogo *jogo, int linha, int coluna);
void decorarMapa(Jogo *jogo);

int podeConstruirTorre(Jogo *jogo, int linha, int coluna);
int posicionarTorre(Jogo *jogo, int linha, int coluna, char tipo);

void desenharMapa(Jogo *jogo, int posX, int posY, int larguraCelula, int alturaCelula);

#endif
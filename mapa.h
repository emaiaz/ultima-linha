#ifndef MAPA_H
#define MAPA_H

#include "jogo.h"

void inicializarMapa(Jogo *jogo);

void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna);

void criarCaminho(Jogo *jogo);

void exibirMapa(Jogo *jogo);

void decorarMapa(Jogo *jogo);

#endif
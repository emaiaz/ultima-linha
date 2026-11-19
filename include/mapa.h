
#ifndef MAPA_H
#define MAPA_H

#include "jogo.h"

void desenharCelula(char simbolo);
void inicializarMapa(Jogo *jogo);
void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna);
void criarCaminho(Jogo *jogo);
int ehCaminho(Jogo *jogo, int linha, int coluna);
void decorarMapa(Jogo *jogo);

void exibirCabecalho(void);
void exibirStatus(Jogo *jogo);
void exibirMapa(Jogo *jogo);
void exibirLegenda(void);
void exibirMenuPrincipal(void);

int podeConstruirTorre(Jogo *jogo, int linha, int coluna);
int posicionarTorre(Jogo *jogo, int linha, int coluna, char tipo);
void exibirMapaConstrucao(Jogo *jogo);

#endif

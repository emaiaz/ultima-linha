#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "jogo.h"
#include "onda.h"
#include "torre.h"

#define MAX_RECORDES 10
#define TAM_NOME_JOGADOR 32

typedef struct
{
    char nome[TAM_NOME_JOGADOR];
    int pontuacao;
    int onda;
    int vitoria;
    int vidaBase;
    int moedas;
    char data[20];
} Recorde;

typedef struct
{
    int partidas;
    int vitorias;
    int derrotas;
    int melhorOnda;
    int maiorPontuacao;
} Estatisticas;

int salvarPartida(const char *caminho, const Jogo *jogo, const Onda *onda,
                  const Inimigo inimigos[], const Torre torres[],
                  int vitoria, int derrota);
int carregarPartida(const char *caminho, Jogo *jogo, Onda *onda,
                    Inimigo inimigos[], Torre torres[],
                    int *vitoria, int *derrota);
int carregarRecordes(Recorde recordes[], int capacidade);
void carregarEstatisticas(Estatisticas *estatisticas);
int registrarPartida(const char *nome, int pontuacao, int onda,
                     int vitoria, int vidaBase, int moedas);

#endif

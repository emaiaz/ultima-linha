
#ifndef TORRE_H
#define TORRE_H

#include "inimigo.h"

#define MAX_TORRES 50

typedef enum
{
    TORRE_ARQUEIRO,
    TORRE_GELO,
    TORRE_TROVAO
} TipoTorre;

typedef struct
{
    int linha;
    int coluna;
    int dano;
    int custo;
    float alcance;
    float intervaloAtaque;
    float tempoRecarga;
    TipoTorre tipo;
    int ativo;
} Torre;

void inicializarTorres(Torre torres[], int quantidade);

int construirTorre(Torre torres[], int quantidade,
                   Jogo *jogo, int linha, int coluna,
                   TipoTorre tipo);

void atualizarTorres(Torre torres[], int quantidadeTorres,
                     Inimigo inimigos[], int quantidadeInimigos,
                     Jogo *jogo, float deltaTime,
                     int posX, int posY,
                     int larguraCelula, int alturaCelula);

void desenharTorres(Torre torres[], int quantidade,
                    int posX, int posY,
                    int larguraCelula, int alturaCelula);

#endif

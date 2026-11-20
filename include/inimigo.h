
#ifndef INIMIGO_H
#define INIMIGO_H

#include "jogo.h"

#define MAX_INIMIGOS 30

typedef enum
{
    DRAUGR,
    FENRIR,
    TROLL,
    GUERREIRO_HEL,
    JORMUNGANDR
} TipoInimigo;

typedef struct
{
    float progresso;
    float vida;
    float vidaMaxima;
    float velocidade;

    int danoBase;
    int recompensa;
    int ativo;
    int corrompido;

    TipoInimigo tipo;
} Inimigo;

void inicializarInimigos(Inimigo inimigos[], int quantidade);

int criarInimigo(Inimigo inimigos[], int quantidade,
                 TipoInimigo tipo, int corrompido);

void atualizarInimigos(Inimigo inimigos[], int quantidade,
                       Jogo *jogo, float deltaTime);

void desenharInimigos(Inimigo inimigos[], int quantidade,
                      Jogo *jogo, int posX, int posY,
                      int larguraCelula, int alturaCelula);

#endif


#ifndef ONDA_H
#define ONDA_H

#include "inimigo.h"

#define INTERVALO_SPAWN 1.3f

typedef struct
{
    int iniciada;
    int inimigosCriados;
    int totalInimigos;
    float tempoSpawn;
} Onda;

void inicializarOnda(Onda *onda);
int iniciarOnda(Onda *onda, int numeroOnda);
void atualizarOnda(Onda *onda, int numeroOnda, Inimigo inimigos[], int quantidade, float deltaTime);
int verificarFimOnda(Onda *onda, Inimigo inimigos[], int quantidade);
int quantidadeDaOnda(int numeroOnda);
TipoInimigo tipoDaOnda(int numeroOnda, int indice);
int contarInimigosAtivos(Inimigo inimigos[], int quantidade);

#endif

#ifndef JOGO_H
#define JOGO_H

#define LINHAS 10
#define COLUNAS 20
#define MAX_CAMINHO (LINHAS * COLUNAS)

typedef struct
{
    int linha;
    int coluna;
} Posicao;

typedef struct 
{
    char mapa[LINHAS][COLUNAS];
    Posicao caminho[MAX_CAMINHO];
    int quantidadeCaminho;

    int moedas;
    int vidaBase;
    int onda;
    int turno;
} Jogo;


#endif

#include <stdio.h>
#include "mapa.h"

void inicializarMapa(Jogo *jogo)
{
    int linha, coluna;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            jogo->mapa[linha][coluna] = '.';
        }
    }
}

void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna)
{
    int indice = jogo->quantidadeCaminho;

    jogo->caminho[indice].linha = linha;
    jogo->caminho[indice].coluna = coluna;


    if(jogo->quantidadeCaminho < MAX_CAMINHO){
        jogo->quantidadeCaminho++;
    }
}
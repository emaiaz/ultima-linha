#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapa.h"

void inicializarMapa(Jogo *jogo)
{
    int linha, coluna;
    jogo->quantidadeCaminho = 0;

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
    if (jogo->quantidadeCaminho < MAX_CAMINHO)
    {
        int indice = jogo->quantidadeCaminho;

        jogo->caminho[indice].linha = linha;
        jogo->caminho[indice].coluna = coluna;

        jogo->quantidadeCaminho++;
    }
}

void criarCaminho(Jogo *jogo)
{
    int linha = 1;
    int coluna;

    for (coluna = 0; coluna <= 18; coluna++)
    {
        adicionarPosicaoCaminho(jogo, linha, coluna);
        jogo->mapa[linha][coluna] = '=';
    }

    for (linha = 2; linha <= 8; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 18);
        jogo->mapa[linha][18] = '=';
    }

    for (coluna = 17; coluna >= 2; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 8, coluna);
        jogo->mapa[8][coluna] = '=';
    }

    for (linha = 7; linha >= 3; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 2);
        jogo->mapa[linha][2] = '=';
    }

    for (coluna = 3; coluna <= 16; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 3, coluna);
        jogo->mapa[3][coluna] = '=';
    }

    for (linha = 4; linha <= 6; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 16);
        jogo->mapa[linha][16] = '=';
    }

    for (coluna = 15; coluna >= 5; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 6, coluna);
        jogo->mapa[6][coluna] = '=';
    }

    for (linha = 5; linha >= 4; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 5);
        jogo->mapa[linha][5] = '=';
    }

    for (coluna = 6; coluna <= 10; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 4, coluna);
        jogo->mapa[4][coluna] = '=';
    }

    jogo->mapa[jogo->caminho[0].linha][jogo->caminho[0].coluna] = 'E';
    jogo->mapa[jogo->caminho[jogo->quantidadeCaminho - 1].linha][jogo->caminho[jogo->quantidadeCaminho - 1].coluna] = 'B';
}

void exibirMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            printf("%c", jogo->mapa[linha][coluna]);
        }
        printf("\n");
    }
}

void decorarMapa(Jogo *jogo)
{
    int i;
    int ehCaminho;
    int linha;
    int coluna;

    for (linha = 0; linha < LINHAS; linha++)
    {

        for (coluna = 0; coluna < COLUNAS; coluna++)
        {

            ehCaminho = 0;

            for (i = 0; i < jogo->quantidadeCaminho; i++)
            {

                if (linha == jogo->caminho[i].linha && coluna == jogo->caminho[i].coluna)
                {
                    ehCaminho = 1;
                    break;
                }
            }

            if (ehCaminho == 0 && jogo->mapa[linha][coluna] == '.')
            {
                int numero = rand() % 10;

                if (linha <= 2)
                {
                    if (numero <= 5)
                    {
                        jogo->mapa[linha][coluna] = 'F';
                    }
                    else if (numero == 6)
                    {
                        jogo->mapa[linha][coluna] = '#';
                    }
                    else if (numero == 7)
                    {
                        jogo->mapa[linha][coluna] = 'V';
                    }
                }
                else if (linha <= 6)
                {
                    if (numero <= 2)
                    {
                        jogo->mapa[linha][coluna] = '#';
                    }
                    else if (numero <= 5)
                    {
                        jogo->mapa[linha][coluna] = 'V';
                    }
                    else if (numero == 6)
                    {
                        jogo->mapa[linha][coluna] = 'F';
                    }
                }
                else
                {
                    if (numero <= 4)
                    {
                        jogo->mapa[linha][coluna] = '^';
                    }
                    else if (numero <= 6)
                    {
                        jogo->mapa[linha][coluna] = '#';
                    }
                    else if (numero == 7)
                    {
                        jogo->mapa[linha][coluna] = 'V';
                    }
                }
            }
        }
    }
}
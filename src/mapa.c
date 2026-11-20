
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "mapa.h"

/* =========================================================
   CORES DO MAPA
   ========================================================= */

#define GRAMA_BASE       ((Color){54, 91, 57, 255})
#define GRAMA_CLARA      ((Color){72, 112, 68, 255})
#define GRAMA_ESCURO     ((Color){42, 73, 48, 255})

#define TERRA            ((Color){119, 86, 55, 255})
#define TERRA_CLARA      ((Color){145, 108, 70, 255})

#define PEDRA            ((Color){75, 77, 72, 255})
#define PEDRA_CLARA      ((Color){120, 121, 110, 255})

#define VERDE_ARVORE     ((Color){24, 65, 39, 255})
#define VERDE_ARVORE_CLARO ((Color){39, 91, 48, 255})

#define TRONCO           ((Color){91, 62, 39, 255})
#define RUNA             ((Color){75, 183, 190, 255})

/* =========================================================
   INICIALIZAÇÃO DO MAPA
   ========================================================= */

void inicializarMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    jogo->quantidadeCaminho = 0;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            jogo->mapa[linha][coluna] = '.';
        }
    }

    criarCaminho(jogo);
    decorarMapa(jogo);
}

/* =========================================================
   CAMINHO
   ========================================================= */

void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna)
{
    if (jogo->quantidadeCaminho >= MAX_CAMINHO)
    {
        return;
    }

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
    {
        return;
    }

    jogo->caminho[jogo->quantidadeCaminho].linha = linha;
    jogo->caminho[jogo->quantidadeCaminho].coluna = coluna;

    jogo->quantidadeCaminho++;
}

void criarCaminho(Jogo *jogo)
{
    int coluna;
    int linha;

    jogo->quantidadeCaminho = 0;

    /* Parte superior */
    for (coluna = 0; coluna <= 18; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 1, coluna);
    }

    /* Lateral direita */
    for (linha = 2; linha <= 8; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 18);
    }

    /* Parte inferior */
    for (coluna = 17; coluna >= 2; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 8, coluna);
    }

    /* Lateral esquerda */
    for (linha = 7; linha >= 3; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 2);
    }

    /* Parte interna superior */
    for (coluna = 3; coluna <= 16; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 3, coluna);
    }

    /* Lateral interna direita */
    for (linha = 4; linha <= 6; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 16);
    }

    /* Parte interna inferior */
    for (coluna = 15; coluna >= 5; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 6, coluna);
    }

    /* Lateral interna esquerda */
    for (linha = 5; linha >= 4; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 5);
    }

    /* Caminho central */
    for (coluna = 6; coluna <= 10; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 4, coluna);
    }

    /* Marca entrada e destino */
    if (jogo->quantidadeCaminho > 0)
    {
        Posicao entrada = jogo->caminho[0];
        Posicao base = jogo->caminho[jogo->quantidadeCaminho - 1];

        jogo->mapa[entrada.linha][entrada.coluna] = 'E';
        jogo->mapa[base.linha][base.coluna] = 'B';
    }
}

int ehCaminho(Jogo *jogo, int linha, int coluna)
{
    int i;

    for (i = 0; i < jogo->quantidadeCaminho; i++)
    {
        if (jogo->caminho[i].linha == linha &&
            jogo->caminho[i].coluna == coluna)
        {
            return 1;
        }
    }

    return 0;
}

/* =========================================================
   DECORAÇÃO DO TERRENO
   ========================================================= */

void decorarMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    srand(10);

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            if (ehCaminho(jogo, linha, coluna))
            {
                continue;
            }

            int sorteio = rand() % 100;

            if (sorteio < 9)
            {
                jogo->mapa[linha][coluna] = 'V';
            }
            else if (sorteio < 14)
            {
                jogo->mapa[linha][coluna] = '#';
            }
            else if (sorteio < 18)
            {
                jogo->mapa[linha][coluna] = 'L';
            }
            else if (sorteio < 21)
            {
                jogo->mapa[linha][coluna] = 'R';
            }
        }
    }
}

/* =========================================================
   CONSTRUÇÃO DE TORRES
   ========================================================= */

int podeConstruirTorre(Jogo *jogo, int linha, int coluna)
{
    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
    {
        return 0;
    }

    return jogo->mapa[linha][coluna] == '.';
}

int posicionarTorre(Jogo *jogo, int linha, int coluna, char tipo)
{
    if (!podeConstruirTorre(jogo, linha, coluna))
    {
        return 0;
    }

    if (tipo != 'T' && tipo != 'G')
    {
        return 0;
    }

    jogo->mapa[linha][coluna] = tipo;

    return 1;
}

/* =========================================================
   ELEMENTOS VISUAIS
   ========================================================= */

static void desenharArvore(float x, float y, float tamanho)
{
    DrawEllipse(x, y + tamanho * 0.75f,
                tamanho * 0.48f, tamanho * 0.16f,
                (Color){20, 42, 27, 100});

    DrawRectangle(x - tamanho * 0.09f,
                  y + tamanho * 0.30f,
                  tamanho * 0.18f,
                  tamanho * 0.55f,
                  TRONCO);

    DrawCircle(x, y, tamanho * 0.35f, VERDE_ARVORE);
    DrawCircle(x - tamanho * 0.15f,
               y + tamanho * 0.08f,
               tamanho * 0.23f,
               VERDE_ARVORE_CLARO);

    DrawCircle(x + tamanho * 0.15f,
               y + tamanho * 0.10f,
               tamanho * 0.22f,
               VERDE_ARVORE);

    DrawCircle(x, y - tamanho * 0.15f,
               tamanho * 0.19f,
               VERDE_ARVORE_CLARO);
}

static void desenharPedra(float x, float y, float tamanho)
{
    DrawEllipse(x, y + tamanho * 0.28f,
                tamanho * 0.55f, tamanho * 0.18f,
                (Color){20, 30, 20, 100});

    DrawPoly((Vector2){x, y},
             6,
             tamanho * 0.48f,
             0,
             PEDRA);

    DrawCircle(x - tamanho * 0.10f,
               y - tamanho * 0.08f,
               tamanho * 0.10f,
               PEDRA_CLARA);
}

static void desenharTronco(float x, float y)
{
    DrawEllipse(x, y + 10, 22, 7,
                (Color){20, 30, 20, 100});

    DrawLineEx((Vector2){x - 17, y},
               (Vector2){x + 17, y + 5},
               10,
               TRONCO);

    DrawCircle(x - 17, y, 5, (Color){126, 88, 51, 255});
    DrawCircle(x + 17, y + 5, 5, (Color){126, 88, 51, 255});
}

static void desenharRuna(float x, float y)
{
    DrawEllipse(x, y + 13, 13, 5,
                (Color){20, 30, 20, 100});

    DrawCircle(x, y, 11, PEDRA);
    DrawCircle(x, y, 7, RUNA);

    DrawLineEx((Vector2){x - 3, y + 4},
               (Vector2){x + 3, y - 4},
               2,
               WHITE);

    DrawLineEx((Vector2){x - 3, y - 1},
               (Vector2){x + 3, y + 1},
               2,
               WHITE);
}

static void desenharEntrada(float x, float y)
{
    DrawCircle(x, y, 19, (Color){100, 57, 38, 255});
    DrawCircle(x, y, 13, (Color){180, 125, 65, 255});
    DrawCircle(x, y, 7, (Color){230, 190, 105, 255});

    DrawText("E", x - 5, y - 8, 16, WHITE);
}

static void desenharYggdrasil(float x, float y)
{
    /* Sombra */
    DrawEllipse(x, y + 27, 31, 10,
                (Color){15, 30, 18, 120});

    /* Tronco */
    DrawRectangle(x - 7, y - 8, 14, 39,
                  (Color){104, 67, 39, 255});

    /* Copa */
    DrawCircle(x, y - 26, 23,
               (Color){32, 105, 58, 255});

    DrawCircle(x - 14, y - 17, 16,
               (Color){49, 133, 68, 255});

    DrawCircle(x + 14, y - 17, 16,
               (Color){39, 117, 61, 255});

    DrawCircle(x, y - 39, 15,
               (Color){68, 151, 79, 255});

    /* Brilho */
    DrawCircle(x - 5, y - 41, 4,
               (Color){126, 190, 105, 255});

    DrawText("YGGDRASIL",
             x - 39,
             y + 33,
             10,
             (Color){238, 220, 161, 255});
}

/* =========================================================
   CAMINHO CONTÍNUO
   ========================================================= */

static void desenharCaminho(Jogo *jogo, int posX, int posY,
                            int larguraCelula, int alturaCelula)
{
    int i;

    /* Borda de pedra do caminho */
    for (i = 0; i < jogo->quantidadeCaminho - 1; i++)
    {
        Vector2 inicio = {
            posX + jogo->caminho[i].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        Vector2 fim = {
            posX + jogo->caminho[i + 1].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i + 1].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        DrawLineEx(inicio, fim, 43, PEDRA);
    }

    /* Arredonda as curvas */
    for (i = 0; i < jogo->quantidadeCaminho; i++)
    {
        Vector2 centro = {
            posX + jogo->caminho[i].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        DrawCircleV(centro, 21.5f, PEDRA);
    }

    /* Interior de terra */
    for (i = 0; i < jogo->quantidadeCaminho - 1; i++)
    {
        Vector2 inicio = {
            posX + jogo->caminho[i].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        Vector2 fim = {
            posX + jogo->caminho[i + 1].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i + 1].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        DrawLineEx(inicio, fim, 35, TERRA);
    }

    for (i = 0; i < jogo->quantidadeCaminho; i++)
    {
        Vector2 centro = {
            posX + jogo->caminho[i].coluna * larguraCelula
                 + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula
                 + alturaCelula / 2.0f
        };

        DrawCircleV(centro, 17.5f, TERRA);
    }

    /* Pequenas pedras decorativas */
    for (i = 0; i < jogo->quantidadeCaminho; i += 5)
    {
        float x = posX +
                  jogo->caminho[i].coluna * larguraCelula +
                  larguraCelula / 2.0f;

        float y = posY +
                  jogo->caminho[i].linha * alturaCelula +
                  alturaCelula / 2.0f;

        DrawCircle(x - 19, y - 8, 2, PEDRA_CLARA);
        DrawCircle(x + 19, y + 7, 2, PEDRA_CLARA);
    }
}

/* =========================================================
   OBJETOS DO MAPA
   ========================================================= */

static void desenharObjetos(Jogo *jogo, int posX, int posY,
                            int larguraCelula, int alturaCelula)
{
    int linha;
    int coluna;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            float x = posX + coluna * larguraCelula +
                      larguraCelula / 2.0f;

            float y = posY + linha * alturaCelula +
                      alturaCelula / 2.0f;

            switch (jogo->mapa[linha][coluna])
            {
                case 'V':
                    desenharArvore(x, y + 3, 35);
                    break;

                case '#':
                    desenharPedra(x, y, 30);
                    break;

                case 'L':
                    desenharTronco(x, y);
                    break;

                case 'R':
                    desenharRuna(x, y);
                    break;

                case 'E':
                    desenharEntrada(x, y);
                    break;

                case 'B':
                    desenharYggdrasil(x, y + 5);
                    break;

                case 'T':
                    DrawCircle(x, y, 17, (Color){175, 133, 68, 255});
                    DrawCircle(x, y - 7, 10, (Color){225, 190, 115, 255});
                    break;

                case 'G':
                    DrawCircle(x, y, 17, (Color){78, 135, 195, 255});
                    DrawCircle(x, y - 7, 10, (Color){145, 205, 245, 255});
                    break;
            }
        }
    }
}

/* =========================================================
   DESENHO DO MAPA COMPLETO
   ========================================================= */

void desenharMapa(Jogo *jogo, int posX, int posY,
                  int larguraCelula, int alturaCelula)
{
    int largura = COLUNAS * larguraCelula;
    int altura = LINHAS * alturaCelula;

    Rectangle terreno = {
        posX,
        posY,
        largura,
        altura
    };

    /* Sombra externa */
    DrawRectangleRounded(
        (Rectangle){
            posX - 8,
            posY + 12,
            largura + 16,
            altura + 16
        },
        0.035f,
        8,
        (Color){10, 23, 18, 150}
    );

    /* Parte inferior elevada do terreno */
    DrawRectangleRounded(
        (Rectangle){
            posX - 5,
            posY + 5,
            largura + 10,
            altura + 8
        },
        0.035f,
        8,
        (Color){30, 55, 38, 255}
    );

    /* Superfície única de grama */
    DrawRectangleRounded(
        terreno,
        0.035f,
        8,
        GRAMA_BASE
    );

    /* Textura irregular da grama */
    for (int i = 0; i < 180; i++)
    {
        int x = posX + 8 + (i * 67) % (largura - 16);
        int y = posY + 8 + (i * 43) % (altura - 16);

        int tamanho = 2 + i % 3;

        Color detalhe = (i % 2 == 0)
                        ? GRAMA_CLARA
                        : GRAMA_ESCURO;

        DrawCircle(x, y, tamanho, detalhe);
    }

    /* Contorno externo */
    DrawRectangleRoundedLines(
        terreno,
        0.035f,
        8,
        (Color){24, 49, 31, 255}
    );

    /* Caminho e elementos */
    desenharCaminho(jogo, posX, posY,
                    larguraCelula, alturaCelula);

    desenharObjetos(jogo, posX, posY,
                    larguraCelula, alturaCelula);
}

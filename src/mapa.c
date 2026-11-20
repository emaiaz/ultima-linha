
#include <stdlib.h>
#include "raylib.h"
#include "mapa.h"

#define GRAMA_BASE ((Color){38, 65, 43, 255})
#define GRAMA_CLARA ((Color){58, 86, 49, 255})
#define GRAMA_ESCURO ((Color){23, 43, 34, 255})
#define GRAMA_LUZ ((Color){91, 113, 60, 255})

#define TERRA ((Color){105, 76, 51, 255})
#define TERRA_CLARA ((Color){139, 103, 66, 255})
#define TERRA_ESCURO ((Color){76, 54, 40, 255})

#define PEDRA ((Color){65, 72, 70, 255})
#define PEDRA_CLARA ((Color){116, 125, 115, 255})
#define PEDRA_ESCURO ((Color){38, 46, 44, 255})

#define VERDE_ARVORE ((Color){19, 53, 39, 255})
#define VERDE_ARVORE_CLARO ((Color){39, 91, 57, 255})
#define VERDE_ARVORE_LUZ ((Color){65, 119, 68, 255})

#define TRONCO ((Color){75, 49, 34, 255})
#define TRONCO_CLARO ((Color){119, 81, 49, 255})

#define RUNA ((Color){91, 215, 205, 255})
#define MAGIA ((Color){105, 231, 208, 255})

void inicializarMapa(Jogo *jogo)
{
    if (jogo == NULL)
        return;

    jogo->quantidadeCaminho = 0;

    for (int linha = 0; linha < LINHAS; linha++)
    {
        for (int coluna = 0; coluna < COLUNAS; coluna++)
            jogo->mapa[linha][coluna] = '.';
    }

    criarCaminho(jogo);
    decorarMapa(jogo);
}

void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna)
{
    if (jogo == NULL || jogo->quantidadeCaminho >= MAX_CAMINHO)
        return;

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
        return;

    jogo->caminho[jogo->quantidadeCaminho].linha = linha;
    jogo->caminho[jogo->quantidadeCaminho].coluna = coluna;
    jogo->quantidadeCaminho++;
}

void criarCaminho(Jogo *jogo)
{
    if (jogo == NULL)
        return;

    int coluna;
    int linha;

    jogo->quantidadeCaminho = 0;

    for (coluna = 0; coluna <= 18; coluna++)
        adicionarPosicaoCaminho(jogo, 1, coluna);

    for (linha = 2; linha <= 8; linha++)
        adicionarPosicaoCaminho(jogo, linha, 18);

    for (coluna = 17; coluna >= 2; coluna--)
        adicionarPosicaoCaminho(jogo, 8, coluna);

    for (linha = 7; linha >= 3; linha--)
        adicionarPosicaoCaminho(jogo, linha, 2);

    for (coluna = 3; coluna <= 16; coluna++)
        adicionarPosicaoCaminho(jogo, 3, coluna);

    for (linha = 4; linha <= 6; linha++)
        adicionarPosicaoCaminho(jogo, linha, 16);

    for (coluna = 15; coluna >= 5; coluna--)
        adicionarPosicaoCaminho(jogo, 6, coluna);

    for (linha = 5; linha >= 4; linha--)
        adicionarPosicaoCaminho(jogo, linha, 5);

    for (coluna = 6; coluna <= 10; coluna++)
        adicionarPosicaoCaminho(jogo, 4, coluna);

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
    if (jogo == NULL)
        return 0;

    for (int i = 0; i < jogo->quantidadeCaminho; i++)
    {
        if (jogo->caminho[i].linha == linha &&
            jogo->caminho[i].coluna == coluna)
            return 1;
    }

    return 0;
}

void decorarMapa(Jogo *jogo)
{
    if (jogo == NULL)
        return;

    srand(10);

    for (int linha = 0; linha < LINHAS; linha++)
    {
        for (int coluna = 0; coluna < COLUNAS; coluna++)
        {
            if (ehCaminho(jogo, linha, coluna))
                continue;

            int sorteio = rand() % 100;

            if (sorteio < 9)
                jogo->mapa[linha][coluna] = 'V';
            else if (sorteio < 14)
                jogo->mapa[linha][coluna] = '#';
            else if (sorteio < 18)
                jogo->mapa[linha][coluna] = 'L';
            else if (sorteio < 21)
                jogo->mapa[linha][coluna] = 'R';
        }
    }
}


int podeConstruirTorre(Jogo *jogo, int linha, int coluna)
{
    if (jogo == NULL)
        return 0;

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
        return 0;

    if (ehCaminho(jogo, linha, coluna))
        return 0;

    if (jogo->mapa[linha][coluna] != '.')
        return 0;

    return 1;
}


int posicionarTorre(Jogo *jogo, int linha, int coluna, char tipo)
{
    if (jogo == NULL || !podeConstruirTorre(jogo, linha, coluna))
        return 0;

    if (tipo != 'A' && tipo != 'G' && tipo != 'T')
        return 0;

    jogo->mapa[linha][coluna] = tipo;
    return 1;
}

static void desenharArvore(float x, float y, float tamanho)
{
    DrawEllipse((int)x, (int)(y + tamanho * 0.76f),
                tamanho * 0.52f, tamanho * 0.17f,
                (Color){9, 22, 17, 125});

    DrawRectangle((int)(x - tamanho * 0.09f),
                  (int)(y + tamanho * 0.22f),
                  (int)(tamanho * 0.18f),
                  (int)(tamanho * 0.60f),
                  TRONCO);

    DrawCircleV((Vector2){x, y}, tamanho * 0.37f, VERDE_ARVORE);
    DrawCircleV((Vector2){x - tamanho * 0.17f, y + tamanho * 0.07f},
                tamanho * 0.25f, VERDE_ARVORE_CLARO);
    DrawCircleV((Vector2){x + tamanho * 0.17f, y + tamanho * 0.08f},
                tamanho * 0.24f, VERDE_ARVORE);
    DrawCircleV((Vector2){x, y - tamanho * 0.17f},
                tamanho * 0.22f, VERDE_ARVORE_LUZ);

    DrawCircleV((Vector2){x - tamanho * 0.09f, y - tamanho * 0.20f},
                tamanho * 0.07f, VERDE_ARVORE_CLARO);
}

static void desenharPedra(float x, float y, float tamanho)
{
    DrawEllipse((int)x, (int)(y + tamanho * 0.29f),
                tamanho * 0.56f, tamanho * 0.19f,
                (Color){8, 17, 14, 110});

    DrawPoly((Vector2){x, y}, 7, tamanho * 0.49f, 0, PEDRA_ESCURO);
    DrawPoly((Vector2){x, y - tamanho * 0.04f},
             7, tamanho * 0.42f, 0, PEDRA);

    DrawCircleV((Vector2){x - tamanho * 0.12f, y - tamanho * 0.12f},
                tamanho * 0.11f, PEDRA_CLARA);

    DrawLineEx((Vector2){x - tamanho * 0.16f, y + tamanho * 0.08f},
               (Vector2){x + tamanho * 0.10f, y - tamanho * 0.12f},
               1.5f, PEDRA_CLARA);
}

static void desenharTronco(float x, float y)
{
    DrawEllipse((int)x, (int)(y + 11), 23, 7,
                (Color){8, 17, 14, 110});

    DrawLineEx((Vector2){x - 18, y - 2},
               (Vector2){x + 18, y + 5},
               12, TRONCO);

    DrawLineEx((Vector2){x - 13, y},
               (Vector2){x + 14, y + 5},
               5, TRONCO_CLARO);

    DrawCircleV((Vector2){x - 18, y - 2}, 5, TRONCO_CLARO);
    DrawCircleV((Vector2){x + 18, y + 5}, 5, TRONCO);
}

static void desenharRuna(float x, float y)
{
    DrawCircleV((Vector2){x, y + 3}, 15, PEDRA_ESCURO);
    DrawCircleV((Vector2){x, y}, 12, PEDRA);
    DrawCircleV((Vector2){x, y}, 8, (Color){31, 74, 72, 255});

    DrawLineEx((Vector2){x - 3, y + 5},
               (Vector2){x + 3, y - 5}, 2.5f, RUNA);

    DrawLineEx((Vector2){x - 4, y},
               (Vector2){x + 4, y + 2}, 2.0f, WHITE);

    DrawCircleV((Vector2){x, y}, 2.5f, MAGIA);
}

static void desenharEntrada(float x, float y)
{
    DrawCircleV((Vector2){x, y + 3}, 22, PEDRA_ESCURO);
    DrawCircleV((Vector2){x, y}, 18, (Color){100, 61, 40, 255});
    DrawCircleV((Vector2){x, y}, 13, (Color){174, 119, 65, 255});
    DrawCircleV((Vector2){x, y}, 8, (Color){225, 180, 100, 255});

    DrawCircleV((Vector2){x, y}, 4, (Color){255, 222, 145, 255});

    DrawText("E", (int)x - 5, (int)y - 8, 16, WHITE);
}

static void desenharYggdrasil(float x, float y)
{
    DrawCircleGradient((Vector2){x, y - 15}, 36,
                       (Color){40, 184, 143, 75}, BLANK);

    DrawEllipse((int)x, (int)(y + 24), 30, 9,
                (Color){8, 20, 13, 130});

    DrawRectangle((int)x - 5, (int)y - 12, 10, 37, TRONCO);

    DrawLineEx((Vector2){x - 2, y + 15},
               (Vector2){x - 15, y + 28}, 5, TRONCO_CLARO);

    DrawLineEx((Vector2){x + 2, y + 15},
               (Vector2){x + 15, y + 28}, 5, TRONCO_CLARO);

    DrawCircleV((Vector2){x, y - 27}, 21, VERDE_ARVORE);
    DrawCircleV((Vector2){x - 13, y - 19}, 15, VERDE_ARVORE_CLARO);
    DrawCircleV((Vector2){x + 13, y - 19}, 15, VERDE_ARVORE);
    DrawCircleV((Vector2){x, y - 40}, 13, VERDE_ARVORE_LUZ);

    DrawCircleV((Vector2){x - 5, y - 42}, 4, MAGIA);
    DrawCircleV((Vector2){x + 9, y - 29}, 3, RUNA);

    DrawText("YGGDRASIL", (int)x - 35, (int)y + 32,
             9, (Color){238, 220, 161, 255});
}

static void desenharDetalhesTerreno(int posX, int posY,
                                    int largura, int altura)
{
    for (int i = 0; i < 520; i++)
    {
        int x = posX + 4 + (i * 67 + 19) % (largura - 8);
        int y = posY + 4 + (i * 43 + 13) % (altura - 8);

        float tamanho = 1.5f + (float)(i % 4);

        Color detalhe;

        if (i % 4 == 0)
            detalhe = (Color){91, 119, 65, 65};
        else if (i % 4 == 1)
            detalhe = (Color){12, 30, 23, 100};
        else if (i % 4 == 2)
            detalhe = (Color){118, 130, 73, 55};
        else
            detalhe = (Color){26, 51, 34, 100};

        DrawCircleV((Vector2){(float)x, (float)y}, tamanho, detalhe);
    }

    for (int i = 0; i < 115; i++)
    {
        int x = posX + 8 + (i * 139) % (largura - 16);
        int y = posY + 8 + (i * 83) % (altura - 16);

        DrawLineEx((Vector2){(float)x, (float)y},
                   (Vector2){(float)(x + 4 + i % 5),
                             (float)(y - 3 - i % 4)},
                   1.0f, (Color){112, 139, 76, 95});
    }

    for (int i = 0; i < 35; i++)
    {
        int x = posX + 10 + (i * 173) % (largura - 20);
        int y = posY + 10 + (i * 97) % (altura - 20);

        DrawLineEx((Vector2){(float)x, (float)y},
                   (Vector2){(float)(x - 3), (float)(y - 7)},
                   1.5f, GRAMA_CLARA);

        DrawLineEx((Vector2){(float)x, (float)y},
                   (Vector2){(float)(x + 4), (float)(y - 8)},
                   1.5f, GRAMA_LUZ);
    }
}

static void desenharCaminho(Jogo *jogo, int posX, int posY,
                            int larguraCelula, int alturaCelula)
{
    if (jogo == NULL || jogo->quantidadeCaminho <= 0)
        return;

    for (int i = 0; i < jogo->quantidadeCaminho - 1; i++)
    {
        Vector2 inicio = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        Vector2 fim = {
            posX + jogo->caminho[i + 1].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i + 1].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawLineEx(inicio, fim, 48, PEDRA_ESCURO);
    }

    for (int i = 0; i < jogo->quantidadeCaminho; i++)
    {
        Vector2 centro = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawCircleV(centro, 24, PEDRA_ESCURO);
    }

    for (int i = 0; i < jogo->quantidadeCaminho - 1; i++)
    {
        Vector2 inicio = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        Vector2 fim = {
            posX + jogo->caminho[i + 1].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i + 1].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawLineEx(inicio, fim, 43, PEDRA);
    }

    for (int i = 0; i < jogo->quantidadeCaminho; i++)
    {
        Vector2 centro = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawCircleV(centro, 21.5f, PEDRA);
    }

    for (int i = 0; i < jogo->quantidadeCaminho - 1; i++)
    {
        Vector2 inicio = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        Vector2 fim = {
            posX + jogo->caminho[i + 1].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i + 1].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawLineEx(inicio, fim, 35, TERRA_ESCURO);
    }

    for (int i = 0; i < jogo->quantidadeCaminho; i++)
    {
        Vector2 centro = {
            posX + jogo->caminho[i].coluna * larguraCelula + larguraCelula / 2.0f,
            posY + jogo->caminho[i].linha * alturaCelula + alturaCelula / 2.0f
        };

        DrawCircleV(centro, 17.5f, TERRA);
    }

    for (int i = 0; i < jogo->quantidadeCaminho; i += 3)
    {
        float x = posX + jogo->caminho[i].coluna * larguraCelula +
                  larguraCelula / 2.0f;

        float y = posY + jogo->caminho[i].linha * alturaCelula +
                  alturaCelula / 2.0f;

        DrawCircleV((Vector2){x - 12, y - 7}, 1.8f, TERRA_CLARA);
        DrawCircleV((Vector2){x + 10, y + 8}, 2.0f, TERRA_CLARA);
        DrawCircleV((Vector2){x + 4, y - 11}, 1.2f, PEDRA_CLARA);
    }

    for (int i = 0; i < jogo->quantidadeCaminho; i += 7)
    {
        float x = posX + jogo->caminho[i].coluna * larguraCelula +
                  larguraCelula / 2.0f;

        float y = posY + jogo->caminho[i].linha * alturaCelula +
                  alturaCelula / 2.0f;

        DrawCircleV((Vector2){x - 19, y - 12}, 2.5f, PEDRA_CLARA);
        DrawCircleV((Vector2){x + 19, y + 12}, 2.0f, PEDRA_ESCURO);
    }
}

static void desenharObjetos(Jogo *jogo, int posX, int posY,
                            int larguraCelula, int alturaCelula)
{
    if (jogo == NULL)
        return;

    for (int linha = 0; linha < LINHAS; linha++)
    {
        for (int coluna = 0; coluna < COLUNAS; coluna++)
        {
            float x = posX + coluna * larguraCelula +
                      larguraCelula / 2.0f;

            float y = posY + linha * alturaCelula +
                      alturaCelula / 2.0f;

            switch (jogo->mapa[linha][coluna])
            {
            case 'A':
                DrawCircleV((Vector2){x, y + 3}, 19,
                            (Color){73, 48, 31, 255});
                DrawCircleV((Vector2){x, y}, 15,
                            (Color){151, 105, 55, 255});
                DrawCircleV((Vector2){x, y - 5}, 9,
                            (Color){220, 177, 104, 255});
                break;

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
                DrawCircleV((Vector2){x, y + 3}, 19,
                            (Color){85, 61, 34, 255});
                DrawCircleV((Vector2){x, y}, 15,
                            (Color){175, 133, 68, 255});
                DrawCircleV((Vector2){x, y - 5}, 9,
                            (Color){245, 210, 132, 255});
                break;

            case 'G':
                DrawCircleV((Vector2){x, y + 3}, 19,
                            (Color){38, 75, 115, 255});
                DrawCircleV((Vector2){x, y}, 15,
                            (Color){78, 135, 195, 255});
                DrawCircleV((Vector2){x, y - 5}, 9,
                            (Color){175, 224, 255, 255});
                break;
            }
        }
    }
}

void desenharMapa(Jogo *jogo, int posX, int posY,
                  int larguraCelula, int alturaCelula)
{
    if (jogo == NULL || larguraCelula <= 0 || alturaCelula <= 0)
        return;

    int largura = COLUNAS * larguraCelula;
    int altura = LINHAS * alturaCelula;

    Rectangle terreno = {
        (float)posX,
        (float)posY,
        (float)largura,
        (float)altura
    };

    DrawRectangle(posX, posY, largura, altura, GRAMA_ESCURO);

    DrawRectangleGradientV(
        posX, posY, largura, altura,
        GRAMA_BASE, GRAMA_ESCURO
    );

    desenharDetalhesTerreno(posX, posY, largura, altura);

    desenharCaminho(
        jogo,
        posX,
        posY,
        larguraCelula,
        alturaCelula
    );

    desenharObjetos(
        jogo,
        posX,
        posY,
        larguraCelula,
        alturaCelula
    );

    DrawRectangleLinesEx(
        terreno,
        4,
        (Color){16, 31, 25, 255}
    );

    DrawRectangleLinesEx(
        (Rectangle){
            (float)posX + 4,
            (float)posY + 4,
            (float)largura - 8,
            (float)altura - 8
        },
        1,
        (Color){100, 126, 79, 150}
    );
}

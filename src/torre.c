
#include <math.h>
#include <stddef.h>
#include "torre.h"
#include "mapa.h"
#include "raylib.h"

void inicializarTorres(Torre torres[], int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        torres[i].linha = 0;
        torres[i].coluna = 0;
        torres[i].dano = 0;
        torres[i].custo = 0;
        torres[i].alcance = 0.0f;
        torres[i].intervaloAtaque = 0.0f;
        torres[i].tempoRecarga = 0.0f;
        torres[i].tipo = TORRE_ARQUEIRO;
        torres[i].ativo = 0;
    }
}

int construirTorre(Torre torres[], int quantidade,
                   Jogo *jogo, int linha, int coluna,
                   TipoTorre tipo)
{
    if (torres == NULL || jogo == NULL)
    {
        return 0;
    }

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
    {
        return 0;
    }

    int custo;
    char simbolo;

    switch (tipo)
    {
    case TORRE_ARQUEIRO:
        custo = 50;
        simbolo = 'A';
        break;

    case TORRE_GELO:
        custo = 65;
        simbolo = 'G';
        break;

    case TORRE_TROVAO:
        custo = 90;
        simbolo = 'T';
        break;

    default:
        return 0;
    }

    if (jogo->moedas < custo ||
        !podeConstruirTorre(jogo, linha, coluna))
    {
        return 0;
    }

    int indice = -1;

    for (int i = 0; i < quantidade; i++)
    {
        if (!torres[i].ativo)
        {
            indice = i;
            break;
        }
    }

    if (indice == -1)
    {
        return 0;
    }

    if (!posicionarTorre(jogo, linha, coluna, simbolo))
    {
        return 0;
    }

    Torre novaTorre = {0};

    novaTorre.linha = linha;
    novaTorre.coluna = coluna;
    novaTorre.tipo = tipo;
    novaTorre.custo = custo;
    novaTorre.ativo = 1;

    switch (tipo)
    {
    case TORRE_ARQUEIRO:
        novaTorre.dano = 20;
        novaTorre.alcance = 150.0f;
        novaTorre.intervaloAtaque = 0.8f;
        break;

    case TORRE_GELO:
        novaTorre.dano = 10;
        novaTorre.alcance = 130.0f;
        novaTorre.intervaloAtaque = 1.1f;
        break;

    case TORRE_TROVAO:
        novaTorre.dano = 35;
        novaTorre.alcance = 180.0f;
        novaTorre.intervaloAtaque = 1.5f;
        break;

    default:
        return 0;
    }

    torres[indice] = novaTorre;
    jogo->moedas -= custo;

    return 1;
}

void atualizarTorres(Torre torres[], int quantidadeTorres,
                     Inimigo inimigos[], int quantidadeInimigos,
                     Jogo *jogo, float deltaTime,
                     int posX, int posY,
                     int larguraCelula, int alturaCelula)
{
    if (torres == NULL || inimigos == NULL || jogo == NULL || deltaTime <= 0.0f)
    {
        return;
    }

    for (int i = 0; i < quantidadeTorres; i++)
    {
        if (!torres[i].ativo)
        {
            continue;
        }

        torres[i].tempoRecarga -= deltaTime;

        if (torres[i].tempoRecarga > 0.0f)
        {
            continue;
        }

        float torreX = posX + torres[i].coluna * larguraCelula + larguraCelula / 2.0f;

        float torreY = posY + torres[i].linha * alturaCelula + alturaCelula / 2.0f;

        int alvo = -1;
        float maiorProgresso = -1.0f;

        for (int j = 0; j < quantidadeInimigos; j++)
        {
            if (!inimigos[j].ativo)
            {
                continue;
            }

            int indice = (int)inimigos[j].progresso;

            if (indice < 0 || indice >= jogo->quantidadeCaminho - 1)
            {
                continue;
            }

            float fracao = inimigos[j].progresso - indice;

            Posicao atual = jogo->caminho[indice];
            Posicao proxima = jogo->caminho[indice + 1];

            float inimigoX1 = posX + atual.coluna * larguraCelula + larguraCelula / 2.0f;

            float inimigoY1 = posY + atual.linha * alturaCelula + alturaCelula / 2.0f;

            float inimigoX2 = posX + proxima.coluna * larguraCelula + larguraCelula / 2.0f;

            float inimigoY2 = posY + proxima.linha * alturaCelula + alturaCelula / 2.0f;

            float inimigoX = inimigoX1 +
                             (inimigoX2 - inimigoX1) * fracao;

            float inimigoY = inimigoY1 +
                             (inimigoY2 - inimigoY1) * fracao;

            float dx = inimigoX - torreX;
            float dy = inimigoY - torreY;
            float distancia = sqrtf(dx * dx + dy * dy);

            if (distancia <= torres[i].alcance &&
                inimigos[j].progresso > maiorProgresso)
            {
                alvo = j;
                maiorProgresso = inimigos[j].progresso;
            }
        }

        if (alvo != -1)
        {
            inimigos[alvo].vida -= torres[i].dano;

            torres[i].tempoRecarga = torres[i].intervaloAtaque;

            if (inimigos[alvo].vida <= 0.0f)
            {
                inimigos[alvo].vida = 0.0f;
                inimigos[alvo].ativo = 0;

                adicionarMoedas(jogo, inimigos[alvo].recompensa);
            }
        }
    }
}

void desenharTorres(Torre torres[], int quantidade,
                    int posX, int posY,
                    int larguraCelula, int alturaCelula)
{
    if (torres == NULL)
    {
        return;
    }

    for (int i = 0; i < quantidade; i++)
    {
        if (!torres[i].ativo)
        {
            continue;
        }

        float x = posX + torres[i].coluna * larguraCelula + larguraCelula / 2.0f;

        float y = posY + torres[i].linha * alturaCelula + alturaCelula / 2.0f;

        Color cor;
        Color detalhe;

        switch (torres[i].tipo)
        {
        case TORRE_ARQUEIRO:
            cor = (Color){151, 105, 55, 255};
            detalhe = (Color){225, 183, 112, 255};
            break;

        case TORRE_GELO:
            cor = (Color){70, 137, 175, 255};
            detalhe = (Color){173, 225, 245, 255};
            break;

        case TORRE_TROVAO:
            cor = (Color){105, 75, 155, 255};
            detalhe = (Color){205, 176, 255, 255};
            break;

        default:
            continue;
        }

        DrawEllipse((int)x, (int)(y + 7), 24, 13,
                    (Color){10, 15, 12, 120});

        DrawCircle((int)x, (int)y, 20, (Color){35, 38, 32, 255});
        DrawCircle((int)x, (int)y, 16, cor);
        DrawCircle((int)x, (int)y, 9, detalhe);
        DrawCircle((int)x, (int)y, 4, (Color){35, 38, 32, 255});

        DrawCircleLines((int)x, (int)y, 20,
                        (Color){220, 195, 145, 255});

        if (torres[i].tipo == TORRE_ARQUEIRO)
        {
            DrawLineEx(
                (Vector2){x - 8, y + 8},
                (Vector2){x + 8, y - 8},
                3,
                BROWN);
        }
        else if (torres[i].tipo == TORRE_GELO)
        {
            DrawLineEx(
                (Vector2){x - 7, y},
                (Vector2){x + 7, y},
                3,
                WHITE);

            DrawLineEx(
                (Vector2){x, y - 7},
                (Vector2){x, y + 7},
                3,
                WHITE);
        }
        else
        {
            DrawLineEx(
                (Vector2){x - 5, y - 7},
                (Vector2){x + 2, y},
                3,
                WHITE);

            DrawLineEx(
                (Vector2){x + 2, y},
                (Vector2){x - 3, y + 8},
                3,
                WHITE);
        }
    }
}

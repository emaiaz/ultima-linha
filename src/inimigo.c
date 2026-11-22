
#include <stddef.h>
#include <math.h>
#include "raylib.h"
#include "inimigo.h"

static void desenharAura(float x, float y, float raio, int corrompido, float tempo)
{
    if (!corrompido)
        return;

    float pulso = sinf(tempo * 5.0f) * 3.0f;

    DrawCircleV((Vector2){x, y}, raio + 13 + pulso, (Color){105, 20, 180, 18});
    DrawCircleV((Vector2){x, y}, raio + 7 + pulso, (Color){145, 35, 225, 30});
    DrawCircleLines((int)x, (int)y, raio + 9 + pulso, (Color){190, 90, 255, 130});

    for (int i = 0; i < 6; i++)
    {
        float angulo = tempo * 2.5f + i * 1.047f;
        float px = x + cosf(angulo) * (raio + 8);
        float py = y + sinf(angulo) * (raio * 0.7f);

        DrawCircleV((Vector2){px, py}, 3.5f, (Color){155, 55, 230, 110});
        DrawCircleV((Vector2){px, py}, 1.5f, (Color){235, 205, 255, 255});
    }
}

static void desenharParticulasCorrompidas(float x, float y, float tempo, int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        float fase = tempo * 2.0f + i * 1.37f;
        float px = x + sinf(fase) * (11.0f + i * 2.0f);
        float py = y - 10.0f - (float)((int)(tempo * 18.0f + i * 11) % 23);

        DrawCircleV((Vector2){px, py}, 3.5f, (Color){145, 45, 225, 75});
        DrawCircleV((Vector2){px, py}, 1.5f, (Color){225, 190, 255, 255});
    }
}

static void desenharOlho(float x, float y, Color cor)
{
    DrawCircleV((Vector2){x, y}, 4.5f, (Color){15, 10, 20, 255});
    DrawCircleV((Vector2){x, y}, 3.0f, cor);
    DrawCircleV((Vector2){x - 0.5f, y - 0.5f}, 1.2f, WHITE);
}

static void desenharEspinho(float x, float y, float tamanho, Color cor)
{
    DrawTriangle(
        (Vector2){x - tamanho * 0.5f, y + tamanho * 0.3f},
        (Vector2){x, y - tamanho},
        (Vector2){x + tamanho * 0.5f, y + tamanho * 0.3f},
        cor);
}

void inicializarInimigos(Inimigo inimigos[], int quantidade)
{
    if (inimigos == NULL)
        return;

    for (int i = 0; i < quantidade; i++)
    {
        inimigos[i].progresso = 0.0f;
        inimigos[i].vida = 0.0f;
        inimigos[i].vidaMaxima = 0.0f;
        inimigos[i].velocidade = 0.0f;
        inimigos[i].danoBase = 0;
        inimigos[i].recompensa = 0;
        inimigos[i].ativo = 0;
        inimigos[i].corrompido = 0;
        inimigos[i].tipo = DRAUGR;
        inimigos[i].velocidadeOriginal = 0.0f;
        inimigos[i].tempoLentidao = 0.0f;
    }
}

int criarInimigo(Inimigo inimigos[], int quantidade,
                 TipoInimigo tipo, int corrompido)
{
    if (inimigos == NULL)
        return 0;

    for (int i = 0; i < quantidade; i++)
    {
        if (!inimigos[i].ativo)
        {
            inimigos[i].progresso = 0.0f;
            inimigos[i].tipo = tipo;
            inimigos[i].corrompido = corrompido;

            switch (tipo)
            {
            case DRAUGR:
                inimigos[i].vida = 100;
                inimigos[i].velocidade = 2.0f;
                inimigos[i].danoBase = 10;
                inimigos[i].recompensa = 10;
                break;

            case FENRIR:
                inimigos[i].vida = 65;
                inimigos[i].velocidade = 3.5f;
                inimigos[i].danoBase = 10;
                inimigos[i].recompensa = 15;
                break;

            case TROLL:
                inimigos[i].vida = 300;
                inimigos[i].velocidade = 0.8f;
                inimigos[i].danoBase = 25;
                inimigos[i].recompensa = 25;
                break;

            case GUERREIRO_HEL:
                inimigos[i].vida = 220;
                inimigos[i].velocidade = 1.3f;
                inimigos[i].danoBase = 15;
                inimigos[i].recompensa = 20;
                break;

            case JORMUNGANDR:
                inimigos[i].vida = 4500;
                inimigos[i].velocidade = 0.78f;
                inimigos[i].danoBase = 55;
                inimigos[i].recompensa = 200;
                break;

            default:
                return 0;
            }

            if (corrompido)
            {
                inimigos[i].vida *= 1.7f;
                inimigos[i].velocidade *= 1.15f;
                inimigos[i].danoBase = (int)(inimigos[i].danoBase * 1.5f);
                inimigos[i].recompensa += 15;
            }

            inimigos[i].vidaMaxima = inimigos[i].vida;
            inimigos[i].ativo = 1;
            inimigos[i].velocidadeOriginal = inimigos[i].velocidade;
            inimigos[i].tempoLentidao = 0.0f;

            return 1;
        }
    }

    return 0;
}

void atualizarInimigos(Inimigo inimigos[], int quantidade,
                       Jogo *jogo, float deltaTime)
{
    if (inimigos == NULL || jogo == NULL || deltaTime <= 0.0f)
        return;

    if (jogo->quantidadeCaminho < 2)
        return;

    for (int i = 0; i < quantidade; i++)
    {
        if (!inimigos[i].ativo)
            continue;

        if (inimigos[i].tempoLentidao > 0.0f)
        {
            inimigos[i].tempoLentidao -= deltaTime;

            if (inimigos[i].tempoLentidao <= 0.0f)
            {
                inimigos[i].tempoLentidao = 0.0f;
                inimigos[i].velocidade = inimigos[i].velocidadeOriginal;
            }
        }

        inimigos[i].progresso += inimigos[i].velocidade * deltaTime;

        if (inimigos[i].progresso >= jogo->quantidadeCaminho - 1)
        {
            inimigos[i].ativo = 0;
            jogo->vidaBase -= inimigos[i].danoBase;

            if (jogo->vidaBase < 0)
                jogo->vidaBase = 0;
        }
    }
}

static void desenharDraugr(float x, float y, int corrompido)
{
    float tempo = (float)GetTime();
    float movimento = sinf(tempo * 6.0f) * 1.5f;

    Color corpo = corrompido ? (Color){91, 35, 135, 255} : (Color){55, 91, 66, 255};
    Color pele = corrompido ? (Color){154, 91, 190, 255} : (Color){133, 157, 117, 255};
    Color roupa = corrompido ? (Color){48, 22, 70, 255} : (Color){42, 48, 39, 255};
    Color metal = corrompido ? (Color){190, 116, 235, 255} : (Color){125, 133, 119, 255};
    Color olhos = corrompido ? (Color){225, 155, 255, 255} : (Color){255, 65, 45, 255};

    desenharAura(x, y, 18, corrompido, tempo);

    DrawEllipse((int)x, (int)(y + 17), 17, 6, (Color){8, 10, 12, 145});

    DrawLineEx((Vector2){x - 6, y + 5}, (Vector2){x - 9, y + 18 + movimento}, 6, corpo);
    DrawLineEx((Vector2){x + 6, y + 5}, (Vector2){x + 9, y + 18 - movimento}, 6, corpo);

    DrawRectangle((int)x - 10, (int)y + 9, 7, 5, metal);
    DrawRectangle((int)x + 3, (int)y + 9, 7, 5, metal);

    DrawEllipse((int)x, (int)(y + 1), 13, 14, roupa);
    DrawTriangle((Vector2){x - 12, y - 1}, (Vector2){x, y + 17}, (Vector2){x + 12, y - 1}, roupa);

    DrawRectangle((int)x - 11, (int)y - 3, 22, 5, corpo);
    DrawRectangle((int)x - 3, (int)y - 2, 6, 4, metal);

    DrawLineEx((Vector2){x - 9, y - 3}, (Vector2){x - 15, y + 8}, 5, corpo);
    DrawLineEx((Vector2){x + 9, y - 3}, (Vector2){x + 16, y + 7}, 5, corpo);

    DrawCircleV((Vector2){x - 15, y + 9}, 3, pele);
    DrawCircleV((Vector2){x + 16, y + 8}, 3, pele);

    DrawLineEx((Vector2){x + 15, y + 7}, (Vector2){x + 22, y - 13}, 2.5f, metal);
    DrawLineEx((Vector2){x + 22, y - 13}, (Vector2){x + 25, y - 18}, 2, metal);

    DrawCircleV((Vector2){x, y - 11}, 10, corpo);
    DrawCircleV((Vector2){x, y - 10}, 8, pele);

    DrawTriangle((Vector2){x - 8, y - 14}, (Vector2){x - 7, y - 25}, (Vector2){x - 1, y - 14}, corpo);
    DrawTriangle((Vector2){x + 8, y - 14}, (Vector2){x + 7, y - 25}, (Vector2){x + 1, y - 14}, corpo);

    DrawRectangle((int)x - 9, (int)y - 5, 18, 4, roupa);
    DrawLineEx((Vector2){x - 6, y - 16}, (Vector2){x + 6, y - 16}, 2, metal);

    desenharOlho(x - 3.5f, y - 10, olhos);
    desenharOlho(x + 3.5f, y - 10, olhos);

    DrawLineEx((Vector2){x - 5, y + 3}, (Vector2){x + 5, y + 3}, 2, (Color){30, 20, 30, 255});

    DrawLineEx((Vector2){x - 9, y - 2}, (Vector2){x - 13, y + 2}, 2, metal);
    DrawLineEx((Vector2){x + 9, y - 2}, (Vector2){x + 13, y + 2}, 2, metal);

    if (corrompido)
    {
        DrawCircleV((Vector2){x, y - 10}, 13 + sinf(tempo * 7) * 2, (Color){170, 50, 255, 35});
        DrawLineEx((Vector2){x - 8, y + 2}, (Vector2){x - 13, y + 11}, 2, olhos);
        desenharParticulasCorrompidas(x, y, tempo, 4);
    }
}

static void desenharFenrir(float x, float y, int corrompido)
{
    float tempo = (float)GetTime();
    float movimento = sinf(tempo * 9.0f) * 2.0f;

    Color pelo = corrompido ? (Color){91, 36, 139, 255} : (Color){45, 62, 53, 255};
    Color peloClaro = corrompido ? (Color){158, 83, 204, 255} : (Color){116, 132, 101, 255};
    Color sombra = corrompido ? (Color){51, 20, 80, 255} : (Color){29, 39, 33, 255};
    Color olhos = corrompido ? (Color){236, 170, 255, 255} : (Color){255, 190, 55, 255};

    desenharAura(x, y, 22, corrompido, tempo);

    DrawEllipse((int)x, (int)(y + 13), 23, 7, (Color){8, 10, 12, 145});

    DrawLineEx((Vector2){x - 10, y + 3}, (Vector2){x - 14, y + 15 + movimento}, 6, sombra);
    DrawLineEx((Vector2){x + 1, y + 5}, (Vector2){x + 2, y + 16 - movimento}, 6, sombra);
    DrawLineEx((Vector2){x + 12, y + 4}, (Vector2){x + 11, y + 15 + movimento}, 5, sombra);

    DrawEllipse((int)x - 2, (int)y, 21, 11, pelo);
    DrawEllipse((int)x - 6, (int)y - 3, 14, 8, peloClaro);

    DrawTriangle((Vector2){x - 15, y - 3}, (Vector2){x - 25, y - 12}, (Vector2){x - 20, y + 2}, pelo);
    DrawTriangle((Vector2){x - 5, y - 7}, (Vector2){x - 10, y - 17}, (Vector2){x + 1, y - 8}, peloClaro);

    DrawCircleV((Vector2){x + 12, y - 6}, 10, pelo);
    DrawCircleV((Vector2){x + 18, y - 2}, 7, peloClaro);

    DrawTriangle((Vector2){x + 5, y - 10}, (Vector2){x + 5, y - 24}, (Vector2){x + 14, y - 10}, pelo);
    DrawTriangle((Vector2){x + 15, y - 11}, (Vector2){x + 22, y - 24}, (Vector2){x + 24, y - 5}, pelo);

    DrawTriangle((Vector2){x + 17, y - 1}, (Vector2){x + 30, y - 4}, (Vector2){x + 24, y + 5}, peloClaro);
    DrawTriangle((Vector2){x + 20, y + 2}, (Vector2){x + 28, y + 3}, (Vector2){x + 23, y + 7}, sombra);

    DrawLineEx((Vector2){x - 10, y + 4}, (Vector2){x - 13, y + 13}, 5, pelo);
    DrawLineEx((Vector2){x + 1, y + 5}, (Vector2){x + 2, y + 14}, 5, pelo);
    DrawLineEx((Vector2){x + 12, y + 4}, (Vector2){x + 11, y + 13}, 4, pelo);

    DrawLineEx((Vector2){x - 15, y - 2}, (Vector2){x - 23, y - 8}, 4, pelo);
    DrawLineEx((Vector2){x - 23, y - 8}, (Vector2){x - 27, y - 5}, 3, peloClaro);

    DrawTriangle((Vector2){x - 6, y - 7}, (Vector2){x - 12, y - 15}, (Vector2){x - 1, y - 11}, peloClaro);

    DrawLineEx((Vector2){x + 8, y - 13}, (Vector2){x + 13, y - 10}, 2, sombra);
    desenharOlho(x + 15, y - 7, olhos);

    DrawCircleV((Vector2){x + 25, y + 1}, 2, sombra);

    if (corrompido)
    {
        DrawLineEx((Vector2){x - 9, y - 5}, (Vector2){x - 16, y - 13}, 2.5f, olhos);
        DrawLineEx((Vector2){x - 16, y - 13}, (Vector2){x - 21, y - 12}, 2, olhos);
        DrawCircleV((Vector2){x + 15, y - 7}, 7, (Color){180, 65, 255, 35});
        desenharParticulasCorrompidas(x, y, tempo, 5);
    }
}

static void desenharTroll(float x, float y, int corrompido)
{
    float tempo = (float)GetTime();
    float movimento = sinf(tempo * 2.0f) * 1.5f;

    Color corpo = corrompido ? (Color){88, 39, 128, 255} : (Color){75, 96, 61, 255};
    Color sombra = corrompido ? (Color){48, 22, 75, 255} : (Color){42, 57, 40, 255};
    Color pedra = corrompido ? (Color){143, 75, 183, 255} : (Color){125, 137, 101, 255};
    Color pedraClara = corrompido ? (Color){190, 125, 225, 255} : (Color){166, 174, 128, 255};
    Color olhos = corrompido ? (Color){238, 165, 255, 255} : (Color){255, 180, 55, 255};

    desenharAura(x, y, 29, corrompido, tempo);

    DrawEllipse((int)x, (int)(y + 23), 27, 9, (Color){8, 10, 12, 150});

    DrawRectangle((int)x - 17, (int)y + 1, 13, 21, sombra);
    DrawRectangle((int)x + 4, (int)y + 1, 13, 21, sombra);

    DrawRectangle((int)x - 17, (int)y + 15, 13, 6, pedra);
    DrawRectangle((int)x + 4, (int)y + 15, 13, 6, pedra);

    DrawEllipse((int)x, (int)(y - 3), 22, 24, sombra);
    DrawEllipse((int)x, (int)(y - 5), 19, 22, corpo);
    DrawEllipse((int)x, (int)(y - 6), 13, 16, pedra);

    DrawCircleV((Vector2){x - 13, y - 7}, 7, corpo);
    DrawCircleV((Vector2){x + 13, y - 7}, 7, corpo);

    DrawCircleV((Vector2){x, y - 24}, 15, corpo);
    DrawCircleV((Vector2){x - 5, y - 25}, 7, pedra);

    DrawTriangle((Vector2){x - 11, y - 31}, (Vector2){x - 20, y - 47}, (Vector2){x - 3, y - 33}, sombra);
    DrawTriangle((Vector2){x + 11, y - 31}, (Vector2){x + 20, y - 47}, (Vector2){x + 3, y - 33}, sombra);

    DrawLineEx((Vector2){x - 16, y - 10}, (Vector2){x - 27, y + 10}, 11, corpo);
    DrawLineEx((Vector2){x + 16, y - 10}, (Vector2){x + 27, y + 10}, 11, corpo);

    DrawCircleV((Vector2){x - 27, y + 11}, 6, pedra);
    DrawCircleV((Vector2){x + 27, y + 11}, 6, pedra);

    DrawCircleV((Vector2){x - 28, y + 12}, 3, pedraClara);
    DrawCircleV((Vector2){x + 26, y + 12}, 3, pedraClara);

    DrawTriangle((Vector2){x - 12, y - 7}, (Vector2){x - 18, y - 17}, (Vector2){x - 7, y - 13}, pedra);
    DrawTriangle((Vector2){x + 12, y - 7}, (Vector2){x + 18, y - 17}, (Vector2){x + 7, y - 13}, pedra);

    desenharEspinho(x - 12, y - 17, 9, pedraClara);
    desenharEspinho(x + 12, y - 17, 9, pedraClara);

    desenharOlho(x - 5, y - 25, olhos);
    desenharOlho(x + 5, y - 25, olhos);

    DrawLineEx((Vector2){x - 6, y - 14}, (Vector2){x + 6, y - 14}, 3, sombra);
    DrawLineEx((Vector2){x - 4, y - 10}, (Vector2){x, y - 8}, 2, pedraClara);
    DrawLineEx((Vector2){x, y - 8}, (Vector2){x + 4, y - 10}, 2, pedraClara);

    DrawCircleV((Vector2){x - 13, y - 4}, 4, sombra);
    DrawCircleV((Vector2){x + 13, y + 1}, 5, sombra);

    DrawLineEx((Vector2){x - 7, y + 4}, (Vector2){x + 7, y + 4}, 3, pedraClara);

    if (corrompido)
    {
        DrawLineEx((Vector2){x - 14, y - 6}, (Vector2){x - 4, y - 15}, 3, olhos);
        DrawLineEx((Vector2){x + 14, y - 6}, (Vector2){x + 5, y - 15}, 3, olhos);
        DrawCircleV((Vector2){x, y - 5 + movimento}, 9, (Color){165, 50, 255, 35});
        desenharParticulasCorrompidas(x, y, tempo, 6);
    }
}

static void desenharHel(float x, float y, int corrompido)
{
    float tempo = (float)GetTime();
    float flutuacao = sinf(tempo * 3.0f) * 2.0f;

    Color armadura = corrompido ? (Color){68, 29, 112, 255} : (Color){39, 50, 67, 255};
    Color metal = corrompido ? (Color){137, 78, 180, 255} : (Color){117, 135, 151, 255};
    Color metalClaro = corrompido ? (Color){204, 147, 240, 255} : (Color){181, 199, 208, 255};
    Color olhos = corrompido ? (Color){232, 158, 255, 255} : (Color){100, 205, 255, 255};

    desenharAura(x, y, 21, corrompido, tempo);

    DrawEllipse((int)x, (int)(y + 17), 19, 7, (Color){8, 10, 12, 130});

    DrawLineEx((Vector2){x - 6, y + 9 + flutuacao}, (Vector2){x - 8, y + 23 + flutuacao}, 6, armadura);
    DrawLineEx((Vector2){x + 6, y + 9 + flutuacao}, (Vector2){x + 8, y + 23 + flutuacao}, 6, armadura);

    DrawRectangle((int)x - 12, (int)y - 6 + (int)flutuacao, 24, 19, armadura);
    DrawRectangle((int)x - 8, (int)y - 3 + (int)flutuacao, 16, 12, metal);

    DrawTriangle(
        (Vector2){x - 12, y - 3 + flutuacao},
        (Vector2){x, y + 16 + flutuacao},
        (Vector2){x + 12, y - 3 + flutuacao},
        armadura);

    DrawLineEx((Vector2){x - 10, y + 1 + flutuacao}, (Vector2){x + 10, y + 1 + flutuacao}, 2, metalClaro);
    DrawCircleV((Vector2){x, y + 5 + flutuacao}, 4, olhos);

    DrawCircleV((Vector2){x, y - 14 + flutuacao}, 12, armadura);
    DrawCircleV((Vector2){x, y - 15 + flutuacao}, 9, metal);

    DrawTriangle(
        (Vector2){x - 8, y - 20 + flutuacao},
        (Vector2){x - 17, y - 35 + flutuacao},
        (Vector2){x - 3, y - 24 + flutuacao},
        metal);

    DrawTriangle(
        (Vector2){x + 8, y - 20 + flutuacao},
        (Vector2){x + 17, y - 35 + flutuacao},
        (Vector2){x + 3, y - 24 + flutuacao},
        metal);

    DrawRectangle((int)x - 8, (int)y - 7 + (int)flutuacao, 16, 4, armadura);

    DrawLineEx((Vector2){x - 14, y - 6 + flutuacao}, (Vector2){x - 23, y + 9 + flutuacao}, 6, armadura);
    DrawLineEx((Vector2){x + 14, y - 6 + flutuacao}, (Vector2){x + 21, y + 9 + flutuacao}, 6, armadura);

    DrawCircleV((Vector2){x - 23, y + 10 + flutuacao}, 4, metal);
    DrawCircleV((Vector2){x + 21, y + 10 + flutuacao}, 4, metal);

    DrawRectangle((int)x + 16, (int)y - 7 + (int)flutuacao, 9, 20, metal);

    DrawTriangle(
        (Vector2){x + 16, y + 13 + flutuacao},
        (Vector2){x + 25, y + 13 + flutuacao},
        (Vector2){x + 20, y + 24 + flutuacao},
        metalClaro);

    DrawLineEx((Vector2){x + 20, y - 5 + flutuacao}, (Vector2){x + 20, y + 17 + flutuacao}, 1.5f, armadura);

    desenharOlho(x - 4, y - 15 + flutuacao, olhos);
    desenharOlho(x + 4, y - 15 + flutuacao, olhos);

    if (corrompido)
    {
        DrawCircleV((Vector2){x, y + 5 + flutuacao}, 9, (Color){165, 50, 255, 50});
        DrawLineEx((Vector2){x - 14, y - 7}, (Vector2){x - 21, y + 5}, 2, olhos);
        DrawLineEx((Vector2){x + 14, y - 7}, (Vector2){x + 20, y + 5}, 2, olhos);
        desenharParticulasCorrompidas(x, y, tempo, 5);
    }
}

static void desenharJormungandr(float x, float y, int corrompido)
{
    float tempo = (float)GetTime();

    Color corpo = corrompido ? (Color){83, 34, 126, 255} : (Color){27, 77, 66, 255};
    Color escamas = corrompido ? (Color){151, 75, 190, 255} : (Color){75, 148, 112, 255};
    Color escamasClaras = corrompido ? (Color){196, 125, 226, 255} : (Color){112, 177, 130, 255};
    Color barriga = corrompido ? (Color){106, 55, 143, 255} : (Color){151, 139, 92, 255};
    Color olhos = corrompido ? (Color){240, 170, 255, 255} : (Color){255, 196, 55, 255};

    desenharAura(x, y, 38, corrompido, tempo);

    DrawEllipse((int)x, (int)(y + 21), 40, 11, (Color){8, 10, 12, 150});

    for (int i = 7; i >= 0; i--)
    {
        float deslocamento = (float)i * 10.0f;
        float ondulacao = sinf(tempo * 3.0f - i * 0.7f) * 5.0f;
        float px = x - deslocamento;
        float py = y + ((i % 2 == 0) ? -5.0f : 5.0f) + ondulacao;
        float raio = 15.5f - i * 0.7f;

        DrawCircleV((Vector2){px, py + 2}, raio + 2, corpo);
        DrawCircleV((Vector2){px, py + 5}, raio * 0.67f, barriga);
        DrawCircleV((Vector2){px, py - 2}, raio * 0.78f, escamas);

        DrawLineEx(
            (Vector2){px - 6, py - 3},
            (Vector2){px + 6, py - 3},
            2,
            escamasClaras);

        DrawLineEx(
            (Vector2){px - 5, py + 5},
            (Vector2){px + 5, py + 5},
            1.5f,
            barriga);

        if (i % 2 == 0)
            desenharEspinho(px, py - raio + 2, 8, corpo);
    }

    DrawCircleV((Vector2){x + 14, y - 5}, 19, corpo);
    DrawCircleV((Vector2){x + 18, y - 8}, 13, escamas);
    DrawCircleV((Vector2){x + 23, y - 1}, 9, escamasClaras);

    DrawTriangle(
        (Vector2){x + 5, y - 17},
        (Vector2){x + 1, y - 35},
        (Vector2){x + 14, y - 19},
        corpo);

    DrawTriangle(
        (Vector2){x + 20, y - 18},
        (Vector2){x + 31, y - 33},
        (Vector2){x + 30, y - 12},
        corpo);

    DrawTriangle(
        (Vector2){x + 18, y + 1},
        (Vector2){x + 36, y + 5},
        (Vector2){x + 29, y + 12},
        barriga);

    DrawTriangle(
        (Vector2){x + 26, y - 4},
        (Vector2){x + 37, y - 7},
        (Vector2){x + 32, y + 1},
        escamas);

    DrawLineEx(
        (Vector2){x + 18, y + 5},
        (Vector2){x + 32, y + 6},
        3,
        (Color){25, 20, 25, 255});

    DrawCircleV((Vector2){x + 31, y + 6}, 2, (Color){25, 20, 25, 255});

    desenharOlho(x + 21, y - 10, olhos);

    DrawLineEx((Vector2){x + 10, y - 19}, (Vector2){x + 17, y - 16}, 2, escamasClaras);

    if (corrompido)
    {
        DrawLineEx(
            (Vector2){x + 3, y - 17},
            (Vector2){x - 4, y - 27},
            3,
            olhos);

        DrawLineEx(
            (Vector2){x - 4, y - 27},
            (Vector2){x - 12, y - 24},
            2,
            olhos);

        DrawCircleV((Vector2){x + 21, y - 10}, 9, (Color){180, 65, 255, 35});
        desenharParticulasCorrompidas(x, y, tempo, 8);
    }
}

void desenharInimigos(Inimigo inimigos[], int quantidade,
                      Jogo *jogo, int posX, int posY,
                      int larguraCelula, int alturaCelula)
{
    if (inimigos == NULL || jogo == NULL ||
        larguraCelula <= 0 || alturaCelula <= 0)
        return;

    for (int i = 0; i < quantidade; i++)
    {
        if (!inimigos[i].ativo)
            continue;

        int indice = (int)inimigos[i].progresso;

        if (indice < 0 || indice >= jogo->quantidadeCaminho - 1)
            continue;

        float fracao = inimigos[i].progresso - indice;

        Posicao atual = jogo->caminho[indice];
        Posicao proxima = jogo->caminho[indice + 1];

        float x1 = posX + atual.coluna * larguraCelula + larguraCelula / 2.0f;
        float y1 = posY + atual.linha * alturaCelula + alturaCelula / 2.0f;

        float x2 = posX + proxima.coluna * larguraCelula + larguraCelula / 2.0f;
        float y2 = posY + proxima.linha * alturaCelula + alturaCelula / 2.0f;

        float x = x1 + (x2 - x1) * fracao;
        float y = y1 + (y2 - y1) * fracao;

        switch (inimigos[i].tipo)
        {
        case DRAUGR:
            desenharDraugr(x, y, inimigos[i].corrompido);
            break;

        case FENRIR:
            desenharFenrir(x, y, inimigos[i].corrompido);
            break;

        case TROLL:
            desenharTroll(x, y, inimigos[i].corrompido);
            break;

        case GUERREIRO_HEL:
            desenharHel(x, y, inimigos[i].corrompido);
            break;

        case JORMUNGANDR:
            desenharJormungandr(x, y, inimigos[i].corrompido);
            break;

        default:
            break;
        }

        float larguraBarra = inimigos[i].tipo == JORMUNGANDR ? 52.0f : 28.0f;
        float alturaBarra = inimigos[i].tipo == JORMUNGANDR ? 6.0f : 4.0f;

        float porcentagem = 0.0f;

        if (inimigos[i].vidaMaxima > 0.0f)
            porcentagem = inimigos[i].vida / inimigos[i].vidaMaxima;

        if (porcentagem < 0.0f)
            porcentagem = 0.0f;

        if (porcentagem > 1.0f)
            porcentagem = 1.0f;

        float barraY = inimigos[i].tipo == JORMUNGANDR ? y - 43 : y - 35;

        DrawRectangle(
            (int)(x - larguraBarra / 2),
            (int)barraY,
            (int)larguraBarra,
            (int)alturaBarra,
            (Color){25, 22, 30, 230});

        Color corVida = inimigos[i].corrompido
                            ? (Color){177, 70, 245, 255}
                            : (Color){76, 205, 100, 255};

        DrawRectangle(
            (int)(x - larguraBarra / 2),
            (int)barraY,
            (int)(larguraBarra * porcentagem),
            (int)alturaBarra,
            corVida);

        if (inimigos[i].corrompido)
        {
            DrawRectangleLines(
                (int)(x - larguraBarra / 2) - 1,
                (int)barraY - 1,
                (int)larguraBarra + 2,
                (int)alturaBarra + 2,
                (Color){190, 105, 255, 180});
        }

        if (inimigos[i].tempoLentidao > 0.0f)
        {
            DrawCircleLines((int)x, (int)y, 19,
                            (Color){105, 205, 255, 170});
        }
    }
}

void aplicarLentidao(Inimigo inimigos[], int indice,
                     int quantidade, float duracao)
{
    if (inimigos == NULL || indice < 0 || indice >= quantidade)
        return;

    if (!inimigos[indice].ativo || duracao <= 0.0f)
        return;

    inimigos[indice].velocidade =
        inimigos[indice].velocidadeOriginal * 0.6f;

    inimigos[indice].tempoLentidao = duracao;
}

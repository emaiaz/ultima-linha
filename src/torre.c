
#include <math.h>
#include <stddef.h>
#include "raylib.h"
#include "torre.h"
#include "mapa.h"
#include "inimigo.h"

typedef struct
{
    int ativo;
    int tipo;
    int nivel;
    Vector2 inicio;
    Vector2 fim;
    float tempo;
    float duracao;
} EfeitoTorre;

static EfeitoTorre efeitosTorre[256];
static int proximoEfeito = 0;

static void registrarEfeito(int tipo, int nivel, Vector2 inicio, Vector2 fim)
{
    EfeitoTorre *efeito = &efeitosTorre[proximoEfeito];

    efeito->ativo = 1;
    efeito->tipo = tipo;
    efeito->nivel = nivel;
    efeito->inicio = inicio;
    efeito->fim = fim;
    efeito->tempo = 0.0f;
    efeito->duracao = tipo == 2 ? 0.22f : 0.30f;

    proximoEfeito = (proximoEfeito + 1) % 256;
}

static void atualizarEfeitosTorre(float deltaTime)
{
    for (int i = 0; i < 256; i++)
    {
        if (!efeitosTorre[i].ativo)
            continue;

        efeitosTorre[i].tempo += deltaTime;

        if (efeitosTorre[i].tempo >= efeitosTorre[i].duracao)
            efeitosTorre[i].ativo = 0;
    }
}

static void desenharBase(float x, float y, Color cor)
{
    DrawEllipse((int)x, (int)(y + 15), 21, 7, (Color){8, 15, 12, 130});
    DrawCircleV((Vector2){x, y + 5}, 19, (Color){24, 31, 30, 255});
    DrawCircleV((Vector2){x, y + 3}, 16, cor);
    DrawCircleV((Vector2){x, y + 1}, 12, (Color){83, 88, 77, 255});
}

static void desenharNivel(float x, float y, int nivel, Color cor)
{
    for (int i = 0; i < nivel; i++)
    {
        DrawCircleV((Vector2){x - 8 + i * 8, y + 23}, 2.5f, cor);
        DrawCircleV((Vector2){x - 8 + i * 8, y + 23}, 1.2f, WHITE);
    }
}

static void desenharRaioRoxo(Vector2 inicio, Vector2 fim, float intensidade, float variacao)
{
    Vector2 meio1 = {
        inicio.x + (fim.x - inicio.x) * 0.35f + sinf(variacao) * 5.0f,
        inicio.y + (fim.y - inicio.y) * 0.35f};

    Vector2 meio2 = {
        inicio.x + (fim.x - inicio.x) * 0.68f + cosf(variacao * 1.4f) * 5.0f,
        inicio.y + (fim.y - inicio.y) * 0.68f};

    Color aura = (Color){151, 45, 255, (unsigned char)(intensidade * 55)};
    Color raio = (Color){174, 74, 255, 230};
    Color centro = (Color){235, 202, 255, 255};

    DrawLineEx(inicio, meio1, 7.0f, aura);
    DrawLineEx(meio1, meio2, 7.0f, aura);
    DrawLineEx(meio2, fim, 7.0f, aura);

    DrawLineEx(inicio, meio1, 3.0f, raio);
    DrawLineEx(meio1, meio2, 3.0f, raio);
    DrawLineEx(meio2, fim, 3.0f, raio);

    DrawLineEx(inicio, meio1, 1.0f, centro);
    DrawLineEx(meio1, meio2, 1.0f, centro);
    DrawLineEx(meio2, fim, 1.0f, centro);
}

static void desenharFlecha(Vector2 inicio, Vector2 fim, float progresso, int nivel)
{
    float dx = fim.x - inicio.x;
    float dy = fim.y - inicio.y;
    float distancia = sqrtf(dx * dx + dy * dy);

    if (distancia < 1.0f)
        return;

    Vector2 direcao = {dx / distancia, dy / distancia};
    Vector2 perpendicular = {-direcao.y, direcao.x};

    Vector2 posicao = {
        inicio.x + dx * progresso,
        inicio.y + dy * progresso};

    float tamanho = nivel == 1 ? 9.0f : nivel == 2 ? 11.0f
                                                   : 14.0f;

    Vector2 ponta = {
        posicao.x + direcao.x * tamanho,
        posicao.y + direcao.y * tamanho};

    Vector2 traseira = {
        posicao.x - direcao.x * tamanho,
        posicao.y - direcao.y * tamanho};

    DrawLineEx(traseira, ponta, nivel == 3 ? 4.0f : 2.5f,
               (Color){95, 62, 35, 220});

    DrawLineEx(traseira, ponta, 1.2f, (Color){235, 204, 145, 255});

    DrawTriangle(
        (Vector2){ponta.x + direcao.x * 5.0f, ponta.y + direcao.y * 5.0f},
        (Vector2){ponta.x - direcao.x * 5.0f + perpendicular.x * 4.0f,
                  ponta.y - direcao.y * 5.0f + perpendicular.y * 4.0f},
        (Vector2){ponta.x - direcao.x * 5.0f - perpendicular.x * 4.0f,
                  ponta.y - direcao.y * 5.0f - perpendicular.y * 4.0f},
        (Color){210, 211, 203, 255});

    if (nivel >= 2)
    {
        DrawCircleV(posicao, 4.0f, (Color){255, 205, 112, 70});
        DrawCircleV(posicao, 1.5f, (Color){255, 225, 160, 255});
    }

    if (nivel >= 3)
    {
        DrawLineEx(
            (Vector2){traseira.x + perpendicular.x * 3.0f,
                      traseira.y + perpendicular.y * 3.0f},
            ponta, 1.5f, (Color){255, 225, 155, 180});

        DrawCircleV(posicao, 6.0f, (Color){255, 190, 75, 35});
    }
}

static void desenharCristalProjetil(Vector2 inicio, Vector2 fim, float progresso, int nivel, float tempo)
{
    Vector2 posicao = {
        inicio.x + (fim.x - inicio.x) * progresso,
        inicio.y + (fim.y - inicio.y) * progresso};

    float pulso = sinf(tempo * 35.0f) * 2.0f;
    float tamanho = nivel == 1 ? 6.0f : nivel == 2 ? 8.0f
                                                   : 10.0f;

    DrawCircleV(posicao, tamanho + 5.0f + pulso,
                (Color){85, 190, 255, 35});

    DrawCircleV(posicao, tamanho + 2.0f,
                (Color){95, 210, 255, 90});

    DrawTriangle(
        (Vector2){posicao.x, posicao.y - tamanho - pulso},
        (Vector2){posicao.x - tamanho * 0.7f, posicao.y},
        (Vector2){posicao.x + tamanho * 0.7f, posicao.y},
        (Color){83, 175, 245, 255});

    DrawTriangle(
        (Vector2){posicao.x, posicao.y - tamanho * 0.65f},
        (Vector2){posicao.x - tamanho * 0.45f, posicao.y},
        (Vector2){posicao.x + tamanho * 0.45f, posicao.y},
        (Color){225, 250, 255, 255});

    DrawCircleV(posicao, 2.0f, WHITE);

    if (nivel >= 2)
    {
        DrawCircleV(
            (Vector2){posicao.x - 7.0f, posicao.y + 4.0f},
            1.8f, (Color){180, 240, 255, 230});

        DrawCircleV(
            (Vector2){posicao.x + 7.0f, posicao.y - 4.0f},
            1.8f, (Color){180, 240, 255, 230});
    }

    if (nivel >= 3)
    {
        DrawCircleV(posicao, tamanho + 9.0f,
                    (Color){110, 220, 255, 35});

        DrawCircleLines((int)posicao.x, (int)posicao.y,
                        tamanho + 7.0f,
                        (Color){210, 250, 255, 170});
    }
}

static void desenharImpacto(Vector2 posicao, int tipo, int nivel, float progresso)
{
    float expansao = progresso * (nivel >= 3 ? 19.0f : 13.0f);
    float opacidade = (1.0f - progresso) * 255.0f;

    if (tipo == 0)
    {
        DrawCircleV(posicao, expansao + 4.0f,
                    (Color){255, 192, 91, (unsigned char)(opacidade * 0.25f)});

        DrawCircleLines((int)posicao.x, (int)posicao.y,
                        expansao,
                        (Color){255, 221, 153, (unsigned char)opacidade});

        for (int i = 0; i < 5; i++)
        {
            float angulo = i * 1.2566f;
            float raio = expansao * 0.8f;

            DrawCircleV(
                (Vector2){posicao.x + cosf(angulo) * raio,
                          posicao.y + sinf(angulo) * raio},
                2.0f * (1.0f - progresso),
                (Color){255, 214, 139, (unsigned char)opacidade});
        }
    }
    else if (tipo == 1)
    {
        DrawCircleV(posicao, expansao + 5.0f,
                    (Color){88, 201, 255, (unsigned char)(opacidade * 0.18f)});

        DrawCircleLines((int)posicao.x, (int)posicao.y,
                        expansao,
                        (Color){185, 240, 255, (unsigned char)opacidade});

        for (int i = 0; i < 6; i++)
        {
            float angulo = i * 1.0472f;
            float raio = expansao * 0.8f;

            Vector2 cristal = {
                posicao.x + cosf(angulo) * raio,
                posicao.y + sinf(angulo) * raio};

            DrawLineEx(posicao, cristal, 2.0f,
                       (Color){177, 239, 255, (unsigned char)opacidade});
            DrawCircleV(cristal, 2.0f,
                        (Color){235, 252, 255, (unsigned char)opacidade});
        }
    }
    else
    {
        DrawCircleV(posicao, expansao + 9.0f,
                    (Color){150, 40, 255, (unsigned char)(opacidade * 0.22f)});

        DrawCircleLines((int)posicao.x, (int)posicao.y,
                        expansao,
                        (Color){205, 151, 255, (unsigned char)opacidade});

        for (int i = 0; i < 7; i++)
        {
            float angulo = i * 0.8976f;
            Vector2 ponto = {
                posicao.x + cosf(angulo) * expansao,
                posicao.y + sinf(angulo) * expansao};

            DrawLineEx(posicao, ponto, 2.0f,
                       (Color){195, 112, 255, (unsigned char)opacidade});

            DrawCircleV(ponto, 2.5f,
                        (Color){240, 210, 255, (unsigned char)opacidade});
        }
    }
}

static void desenharEfeitosTorre(void)
{
    for (int i = 0; i < 256; i++)
    {
        EfeitoTorre *efeito = &efeitosTorre[i];

        if (!efeito->ativo)
            continue;

        float progresso = efeito->tempo / efeito->duracao;

        if (efeito->tipo == 2)
        {
            desenharRaioRoxo(
                efeito->inicio,
                efeito->fim,
                1.5f,
                (float)GetTime() * 18.0f + i);

            DrawCircleV(efeito->fim, 8.0f,
                        (Color){160, 55, 255, 65});

            DrawCircleV(efeito->fim, 3.0f,
                        (Color){245, 220, 255, 230});

            if (efeito->nivel >= 2)
            {
                Vector2 meio = {
                    (efeito->inicio.x + efeito->fim.x) * 0.5f,
                    (efeito->inicio.y + efeito->fim.y) * 0.5f};

                desenharRaioRoxo(
                    meio,
                    (Vector2){meio.x + sinf((float)GetTime() * 13.0f + i) * 13.0f,
                              meio.y + 15.0f},
                    0.8f,
                    (float)GetTime() * 15.0f);
            }
        }
        else if (progresso < 0.78f)
        {
            float movimento = progresso / 0.78f;

            if (efeito->tipo == 0)
            {
                desenharFlecha(
                    efeito->inicio,
                    efeito->fim,
                    movimento,
                    efeito->nivel);
            }
            else
            {
                desenharCristalProjetil(
                    efeito->inicio,
                    efeito->fim,
                    movimento,
                    efeito->nivel,
                    efeito->tempo);
            }
        }
        else
        {
            float impacto = (progresso - 0.78f) / 0.22f;
            desenharImpacto(
                efeito->fim,
                efeito->tipo,
                efeito->nivel,
                impacto);
        }
    }
}

static void desenharTorreArqueiro(float x, float y, int nivel)
{
    Color madeira = (Color){91, 57, 35, 255};
    Color madeiraClara = (Color){145, 94, 51, 255};
    Color ouro = (Color){207, 158, 72, 255};
    Color ouroClaro = (Color){255, 220, 137, 255};

    desenharBase(x, y, (Color){126, 91, 53, 255});

    if (nivel == 1)
    {
        DrawRectangle((int)x - 10, (int)y - 14, 20, 18, madeira);
        DrawRectangle((int)x - 8, (int)y - 17, 16, 5, madeiraClara);

        DrawTriangle(
            (Vector2){x - 13, y - 15},
            (Vector2){x, y - 29},
            (Vector2){x + 13, y - 15},
            madeira);

        DrawTriangle(
            (Vector2){x - 7, y - 16},
            (Vector2){x, y - 25},
            (Vector2){x + 7, y - 16},
            madeiraClara);

        DrawLineEx((Vector2){x - 9, y - 5},
                   (Vector2){x + 9, y - 5}, 2, ouro);
    }
    else if (nivel == 2)
    {
        DrawRectangle((int)x - 12, (int)y - 19, 24, 24, madeira);
        DrawRectangle((int)x - 9, (int)y - 23, 18, 5, ouro);

        DrawRectangle((int)x - 15, (int)y - 17, 5, 15, madeiraClara);
        DrawRectangle((int)x + 10, (int)y - 17, 5, 15, madeiraClara);

        DrawTriangle(
            (Vector2){x - 17, y - 18},
            (Vector2){x - 12, y - 27},
            (Vector2){x - 7, y - 18},
            ouro);

        DrawTriangle(
            (Vector2){x + 7, y - 18},
            (Vector2){x + 12, y - 27},
            (Vector2){x + 17, y - 18},
            ouro);

        DrawTriangle(
            (Vector2){x - 15, y - 22},
            (Vector2){x, y - 39},
            (Vector2){x + 15, y - 22},
            madeira);

        DrawTriangle(
            (Vector2){x - 9, y - 23},
            (Vector2){x, y - 34},
            (Vector2){x + 9, y - 23},
            madeiraClara);

        DrawLineEx((Vector2){x - 11, y - 8},
                   (Vector2){x + 11, y - 8}, 3, ouro);
    }
    else
    {
        DrawRectangle((int)x - 14, (int)y - 23, 28, 28, madeira);
        DrawRectangle((int)x - 11, (int)y - 27, 22, 5, ouro);

        DrawRectangle((int)x - 18, (int)y - 22, 6, 19, madeiraClara);
        DrawRectangle((int)x + 12, (int)y - 22, 6, 19, madeiraClara);

        DrawRectangle((int)x - 20, (int)y - 27, 10, 5, ouro);
        DrawRectangle((int)x + 10, (int)y - 27, 10, 5, ouro);

        DrawTriangle(
            (Vector2){x - 20, y - 26},
            (Vector2){x, y - 50},
            (Vector2){x + 20, y - 26},
            madeira);

        DrawTriangle(
            (Vector2){x - 12, y - 28},
            (Vector2){x, y - 44},
            (Vector2){x + 12, y - 28},
            madeiraClara);

        DrawCircleV((Vector2){x, y - 35}, 5, ouro);
        DrawCircleV((Vector2){x, y - 35}, 2, ouroClaro);

        DrawLineEx((Vector2){x - 13, y - 13},
                   (Vector2){x + 13, y - 13}, 3, ouro);

        DrawLineEx((Vector2){x - 13, y - 5},
                   (Vector2){x + 13, y - 5}, 2, ouro);

        DrawTriangle(
            (Vector2){x - 5, y - 48},
            (Vector2){x, y - 56},
            (Vector2){x + 5, y - 48},
            ouroClaro);
    }

    if (nivel >= 2)
    {
        DrawCircleV((Vector2){x - 13, y - 17}, 2, ouroClaro);
        DrawCircleV((Vector2){x + 13, y - 17}, 2, ouroClaro);
    }

    if (nivel >= 3)
    {
        DrawCircleV((Vector2){x, y - 35}, 9, (Color){255, 210, 105, 45});
        DrawCircleV((Vector2){x, y - 35}, 3, WHITE);
        DrawCircleV((Vector2){x - 16, y - 29}, 2, ouroClaro);
        DrawCircleV((Vector2){x + 16, y - 29}, 2, ouroClaro);
    }

    desenharNivel(x, y, nivel, ouro);
}

static void desenharTorreGelo(float x, float y, int nivel)
{
    float tempo = (float)GetTime();

    Color azulEscuro = (Color){32, 83, 125, 255};
    Color azul = (Color){65, 151, 203, 255};
    Color azulClaro = (Color){151, 220, 248, 255};
    Color brancoAzulado = (Color){216, 248, 255, 255};

    desenharBase(x, y, (Color){55, 119, 153, 255});

    if (nivel == 1)
    {
        DrawTriangle(
            (Vector2){x - 15, y + 3},
            (Vector2){x - 9, y - 15},
            (Vector2){x - 2, y + 3},
            azulEscuro);

        DrawTriangle(
            (Vector2){x + 15, y + 3},
            (Vector2){x + 9, y - 15},
            (Vector2){x + 2, y + 3},
            azulEscuro);

        DrawTriangle(
            (Vector2){x - 11, y - 5},
            (Vector2){x, y - 34},
            (Vector2){x + 11, y - 5},
            azul);

        DrawTriangle(
            (Vector2){x - 5, y - 8},
            (Vector2){x, y - 28},
            (Vector2){x + 5, y - 8},
            brancoAzulado);

        DrawCircleV((Vector2){x, y - 9}, 5, azulClaro);
    }
    else if (nivel == 2)
    {
        DrawTriangle(
            (Vector2){x - 18, y + 2},
            (Vector2){x - 13, y - 27},
            (Vector2){x - 5, y + 2},
            azulEscuro);

        DrawTriangle(
            (Vector2){x + 18, y + 2},
            (Vector2){x + 13, y - 27},
            (Vector2){x + 5, y + 2},
            azulEscuro);

        DrawTriangle(
            (Vector2){x - 13, y - 7},
            (Vector2){x, y - 39},
            (Vector2){x + 13, y - 7},
            azul);

        DrawTriangle(
            (Vector2){x - 6, y - 9},
            (Vector2){x, y - 32},
            (Vector2){x + 6, y - 9},
            brancoAzulado);

        DrawTriangle(
            (Vector2){x - 17, y - 11},
            (Vector2){x - 13, y - 29},
            (Vector2){x - 7, y - 11},
            azulClaro);

        DrawTriangle(
            (Vector2){x + 17, y - 11},
            (Vector2){x + 13, y - 29},
            (Vector2){x + 7, y - 11},
            azulClaro);

        DrawCircleV((Vector2){x, y - 11}, 8, azulEscuro);
        DrawCircleV((Vector2){x, y - 11}, 5, azulClaro);
    }
    else
    {
        float deslocamento = sinf(tempo * 2.0f) * 2.0f;

        DrawTriangle(
            (Vector2){x - 20, y + 3},
            (Vector2){x - 14, y - 29},
            (Vector2){x - 5, y + 3},
            azulEscuro);

        DrawTriangle(
            (Vector2){x + 20, y + 3},
            (Vector2){x + 14, y - 29},
            (Vector2){x + 5, y + 3},
            azulEscuro);

        DrawTriangle(
            (Vector2){x - 13, y - 7},
            (Vector2){x, y - 49 + deslocamento},
            (Vector2){x + 13, y - 7},
            azul);

        DrawTriangle(
            (Vector2){x - 6, y - 12},
            (Vector2){x, y - 40 + deslocamento},
            (Vector2){x + 6, y - 12},
            brancoAzulado);

        DrawTriangle(
            (Vector2){x - 21, y - 13},
            (Vector2){x - 16, y - 36},
            (Vector2){x - 8, y - 13},
            azulClaro);

        DrawTriangle(
            (Vector2){x + 21, y - 13},
            (Vector2){x + 16, y - 36},
            (Vector2){x + 8, y - 13},
            azulClaro);

        DrawTriangle(
            (Vector2){x - 7, y - 15},
            (Vector2){x - 13, y - 34},
            (Vector2){x - 2, y - 20},
            brancoAzulado);

        DrawTriangle(
            (Vector2){x + 7, y - 15},
            (Vector2){x + 13, y - 34},
            (Vector2){x + 2, y - 20},
            brancoAzulado);

        DrawCircleV((Vector2){x, y - 13}, 11, azulEscuro);
        DrawCircleV((Vector2){x, y - 13}, 7, azulClaro);
        DrawCircleV((Vector2){x, y - 13}, 3, WHITE);

        DrawCircleV((Vector2){x, y - 47 + deslocamento}, 4, WHITE);
        DrawCircleV((Vector2){x, y - 47 + deslocamento}, 8,
                    (Color){170, 235, 255, 65});
    }

    for (int i = 0; i < nivel + 1; i++)
    {
        float angulo = tempo * 1.5f + i * (6.28318f / (nivel + 1));
        float px = x + cosf(angulo) * (13.0f + nivel);
        float py = y - 17 + sinf(angulo) * 7.0f;

        DrawCircleV((Vector2){px, py}, 1.8f, brancoAzulado);
    }

    desenharNivel(x, y, nivel, azulClaro);
}

static void desenharTorreTrovao(float x, float y, int nivel)
{
    float tempo = (float)GetTime();
    float pulso = sinf(tempo * 5.0f);
    float energia = 0.5f + 0.5f * sinf(tempo * 7.0f);

    Color pedra = (Color){48, 38, 66, 255};
    Color pedraClara = (Color){83, 60, 111, 255};
    Color roxoEscuro = (Color){72, 20, 130, 255};
    Color roxo = (Color){139, 45, 230, 255};
    Color violeta = (Color){183, 89, 255, 255};
    Color lilas = (Color){221, 174, 255, 255};
    Color branco = (Color){250, 237, 255, 255};

    desenharBase(x, y, (Color){83, 48, 122, 255});

    if (nivel == 1)
    {
        DrawRectangle((int)x - 10, (int)y - 13, 20, 18, pedra);

        DrawTriangle(
            (Vector2){x - 15, y - 10},
            (Vector2){x, y - 32},
            (Vector2){x + 15, y - 10},
            pedraClara);

        DrawTriangle(
            (Vector2){x - 8, y - 12},
            (Vector2){x, y - 25},
            (Vector2){x + 8, y - 12},
            roxoEscuro);

        DrawCircleV((Vector2){x, y - 13}, 11, (Color){105, 35, 175, 75});
        DrawCircleV((Vector2){x, y - 13}, 8, roxoEscuro);
        DrawCircleV((Vector2){x, y - 13}, 5, roxo);
        DrawCircleV((Vector2){x, y - 13}, 2, branco);

        desenharRaioRoxo(
            (Vector2){x - 10, y - 27},
            (Vector2){x - 16, y - 37},
            0.8f, tempo * 8.0f);

        desenharRaioRoxo(
            (Vector2){x + 10, y - 27},
            (Vector2){x + 16, y - 36},
            0.8f, tempo * 7.0f);
    }
    else if (nivel == 2)
    {
        DrawRectangle((int)x - 12, (int)y - 17, 24, 22, pedra);

        DrawRectangle((int)x - 17, (int)y - 14, 7, 18, pedraClara);
        DrawRectangle((int)x + 10, (int)y - 14, 7, 18, pedraClara);

        DrawTriangle(
            (Vector2){x - 19, y - 16},
            (Vector2){x - 13, y - 28},
            (Vector2){x - 7, y - 16},
            roxo);

        DrawTriangle(
            (Vector2){x + 7, y - 16},
            (Vector2){x + 13, y - 28},
            (Vector2){x + 19, y - 16},
            roxo);

        DrawTriangle(
            (Vector2){x - 15, y - 18},
            (Vector2){x, y - 39},
            (Vector2){x + 15, y - 18},
            pedraClara);

        DrawTriangle(
            (Vector2){x - 8, y - 19},
            (Vector2){x, y - 33},
            (Vector2){x + 8, y - 19},
            roxoEscuro);

        DrawCircleV((Vector2){x, y - 15}, 15, (Color){139, 45, 230, 45});
        DrawCircleV((Vector2){x, y - 15}, 11, roxoEscuro);
        DrawCircleV((Vector2){x, y - 15}, 7, roxo);
        DrawCircleV((Vector2){x, y - 15}, 3, branco);

        DrawLineEx((Vector2){x - 12, y - 6},
                   (Vector2){x + 12, y - 6}, 2, violeta);

        desenharRaioRoxo(
            (Vector2){x - 14, y - 26},
            (Vector2){x - 22, y - 39},
            1.2f, tempo * 8.0f);

        desenharRaioRoxo(
            (Vector2){x + 14, y - 26},
            (Vector2){x + 22, y - 39},
            1.2f, tempo * 9.0f);

        for (int i = 0; i < 4; i++)
        {
            float angulo = tempo * 2.0f + i * 1.5707f;
            float px = x + cosf(angulo) * 20.0f;
            float py = y - 22 + sinf(angulo) * 13.0f;

            DrawCircleV((Vector2){px, py}, 4, (Color){139, 45, 230, 65});
            DrawCircleV((Vector2){px, py}, 2, lilas);
        }
    }
    else
    {
        float deslocamento = sinf(tempo * 2.5f) * 3.0f;
        float alturaNucleo = y - 21 + deslocamento;

        DrawCircleV((Vector2){x, y - 29}, 29, (Color){115, 25, 200, 25});
        DrawCircleV((Vector2){x, y - 29}, 23, (Color){145, 40, 245, 30});

        DrawRectangle((int)x - 14, (int)y - 20, 28, 25, pedra);

        DrawRectangle((int)x - 20, (int)y - 17, 8, 21, pedraClara);
        DrawRectangle((int)x + 12, (int)y - 17, 8, 21, pedraClara);

        DrawRectangle((int)x - 22, (int)y - 23, 12, 6, roxo);
        DrawRectangle((int)x + 10, (int)y - 23, 12, 6, roxo);

        DrawTriangle(
            (Vector2){x - 21, y - 20},
            (Vector2){x - 16, y - 36},
            (Vector2){x - 9, y - 20},
            violeta);

        DrawTriangle(
            (Vector2){x + 9, y - 20},
            (Vector2){x + 16, y - 36},
            (Vector2){x + 21, y - 20},
            violeta);

        DrawTriangle(
            (Vector2){x - 20, y - 22},
            (Vector2){x, y - 50},
            (Vector2){x + 20, y - 22},
            pedraClara);

        DrawTriangle(
            (Vector2){x - 12, y - 24},
            (Vector2){x, y - 42},
            (Vector2){x + 12, y - 24},
            roxoEscuro);

        DrawTriangle(
            (Vector2){x - 7, y - 31},
            (Vector2){x, y - 53},
            (Vector2){x + 7, y - 31},
            violeta);

        DrawCircleV((Vector2){x, alturaNucleo}, 19 + pulso, (Color){132, 35, 220, 35});
        DrawCircleV((Vector2){x, alturaNucleo}, 14 + pulso * 0.5f, (Color){155, 55, 255, 90});
        DrawCircleV((Vector2){x, alturaNucleo}, 10, roxoEscuro);
        DrawCircleV((Vector2){x, alturaNucleo}, 7, roxo);
        DrawCircleV((Vector2){x, alturaNucleo}, 4, lilas);
        DrawCircleV((Vector2){x, alturaNucleo}, 2, WHITE);

        DrawLineEx((Vector2){x - 14, y - 7},
                   (Vector2){x + 14, y - 7}, 3, roxo);

        DrawLineEx((Vector2){x - 14, y - 13},
                   (Vector2){x + 14, y - 13}, 2, violeta);

        DrawTriangle(
            (Vector2){x - 6, y - 47},
            (Vector2){x, y - 60},
            (Vector2){x + 6, y - 47},
            lilas);

        desenharRaioRoxo(
            (Vector2){x - 10, y - 39},
            (Vector2){x - 25, y - 53 + sinf(tempo * 4.0f) * 4.0f},
            1.6f, tempo * 11.0f);

        desenharRaioRoxo(
            (Vector2){x + 10, y - 39},
            (Vector2){x + 25, y - 53 + cosf(tempo * 4.0f) * 4.0f},
            1.6f, tempo * 12.0f);

        desenharRaioRoxo(
            (Vector2){x - 18, y - 19},
            (Vector2){x - 28, y - 31 + sinf(tempo * 6.0f) * 3.0f},
            1.2f, tempo * 10.0f);

        desenharRaioRoxo(
            (Vector2){x + 18, y - 19},
            (Vector2){x + 28, y - 31 + cosf(tempo * 6.0f) * 3.0f},
            1.2f, tempo * 9.0f);

        for (int i = 0; i < 8; i++)
        {
            float angulo = tempo * 2.8f + i * 0.7853f;
            float raio = 23.0f + sinf(tempo * 4.0f + i) * 4.0f;
            float px = x + cosf(angulo) * raio;
            float py = y - 28 + sinf(angulo) * raio * 0.65f;

            DrawCircleV((Vector2){px, py}, 5, (Color){145, 40, 245, 45});
            DrawCircleV((Vector2){px, py}, 2.3f, lilas);
            DrawCircleV((Vector2){px, py}, 1.0f, WHITE);
        }

        for (int i = 0; i < 5; i++)
        {
            float fase = tempo * 3.0f + i * 1.2566f;
            float px = x + sinf(fase) * (15.0f + i * 2.0f);
            float py = y - 43 - (float)((int)(tempo * 18.0f + i * 13) % 18);

            DrawCircleV((Vector2){px, py}, 3.5f, (Color){155, 55, 255, 70});
            DrawCircleV((Vector2){px, py}, 1.5f, branco);
        }

        if (energia > 0.65f)
        {
            desenharRaioRoxo(
                (Vector2){x - 3, y - 54},
                (Vector2){x + 4, y - 66},
                1.0f, tempo * 17.0f);
        }
    }

    if (nivel >= 2)
    {
        DrawCircleV((Vector2){x - 14, y - 13}, 3, lilas);
        DrawCircleV((Vector2){x + 14, y - 13}, 3, lilas);
    }

    if (nivel >= 3)
    {
        DrawCircleV((Vector2){x, y - 21}, 24 + pulso * 2.0f,
                    (Color){150, 45, 255, 25});

        DrawCircleLines((int)x, (int)(y - 21), 25 + pulso * 2.0f,
                        (Color){190, 105, 255, 100});

        DrawCircleV((Vector2){x - 16, y - 35}, 2.5f, branco);
        DrawCircleV((Vector2){x + 16, y - 35}, 2.5f, branco);
    }

    desenharNivel(x, y, nivel, violeta);
}

void inicializarTorres(Torre torres[], int quantidade)
{
    if (torres == NULL)
        return;

    for (int i = 0; i < quantidade; i++)
    {
        torres[i] = (Torre){0};
        torres[i].tipo = TORRE_ARQUEIRO;
    }

    for (int i = 0; i < 256; i++)
        efeitosTorre[i].ativo = 0;

    proximoEfeito = 0;
}

int construirTorre(Torre torres[], int quantidade,
                   Jogo *jogo, int linha, int coluna,
                   TipoTorre tipo)
{
    if (torres == NULL || jogo == NULL)
        return 0;

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
        return 0;

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
        return 0;

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
        return 0;

    Torre novaTorre = {0};

    novaTorre.linha = linha;
    novaTorre.coluna = coluna;
    novaTorre.tipo = tipo;
    novaTorre.custo = custo;
    novaTorre.nivel = 1;
    novaTorre.valorInvestido = custo;
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

    if (!posicionarTorre(jogo, linha, coluna, simbolo))
        return 0;

    torres[indice] = novaTorre;
    jogo->moedas -= custo;

    return 1;
}

int selecionarTorre(const Torre torres[], int quantidade,
                    int linha, int coluna)
{
    if (torres == NULL)
        return -1;

    for (int i = 0; i < quantidade; i++)
    {
        if (torres[i].ativo &&
            torres[i].linha == linha &&
            torres[i].coluna == coluna)
            return i;
    }

    return -1;
}

int venderTorre(Torre torres[], int quantidade,
                Jogo *jogo, int indice)
{
    if (torres == NULL || jogo == NULL)
        return 0;

    if (indice < 0 || indice >= quantidade ||
        !torres[indice].ativo)
        return 0;

    Torre *torre = &torres[indice];

    int linha = torre->linha;
    int coluna = torre->coluna;

    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
        return 0;

    char simbolo;

    switch (torre->tipo)
    {
    case TORRE_ARQUEIRO:
        simbolo = 'A';
        break;

    case TORRE_GELO:
        simbolo = 'G';
        break;

    case TORRE_TROVAO:
        simbolo = 'T';
        break;

    default:
        return 0;
    }

    if (jogo->mapa[linha][coluna] != simbolo)
        return 0;

    int reembolso = torre->valorInvestido / 2;

    jogo->mapa[linha][coluna] = '.';
    jogo->moedas += reembolso;

    torre->ativo = 0;

    return 1;
}

void atualizarTorres(Torre torres[], int quantidadeTorres,
                     Inimigo inimigos[], int quantidadeInimigos,
                     Jogo *jogo, float deltaTime,
                     int posX, int posY,
                     int larguraCelula, int alturaCelula)
{
    if (torres == NULL || inimigos == NULL ||
        jogo == NULL || deltaTime <= 0.0f ||
        larguraCelula <= 0 || alturaCelula <= 0)
        return;

    atualizarEfeitosTorre(deltaTime);

    for (int i = 0; i < quantidadeTorres; i++)
    {
        if (!torres[i].ativo)
            continue;

        torres[i].tempoRecarga -= deltaTime;

        if (torres[i].tempoRecarga > 0.0f)
            continue;

        float torreX = posX + torres[i].coluna * larguraCelula +
                       larguraCelula / 2.0f;

        float torreY = posY + torres[i].linha * alturaCelula +
                       alturaCelula / 2.0f;

        int alvo = -1;
        float maiorProgresso = -1.0f;

        for (int j = 0; j < quantidadeInimigos; j++)
        {
            if (!inimigos[j].ativo)
                continue;

            int indice = (int)inimigos[j].progresso;

            if (indice < 0 ||
                indice >= jogo->quantidadeCaminho - 1)
                continue;

            float fracao = inimigos[j].progresso - indice;

            Posicao atual = jogo->caminho[indice];
            Posicao proxima = jogo->caminho[indice + 1];

            float inimigoX1 = posX + atual.coluna * larguraCelula +
                              larguraCelula / 2.0f;

            float inimigoY1 = posY + atual.linha * alturaCelula +
                              alturaCelula / 2.0f;

            float inimigoX2 = posX + proxima.coluna * larguraCelula +
                              larguraCelula / 2.0f;

            float inimigoY2 = posY + proxima.linha * alturaCelula +
                              alturaCelula / 2.0f;

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

        if (alvo == -1)
            continue;

        int indiceAlvo = (int)inimigos[alvo].progresso;

        if (indiceAlvo < 0 ||
            indiceAlvo >= jogo->quantidadeCaminho - 1)
            continue;

        float fracaoAlvo = inimigos[alvo].progresso - indiceAlvo;

        Posicao atualAlvo = jogo->caminho[indiceAlvo];
        Posicao proximaAlvo = jogo->caminho[indiceAlvo + 1];

        float alvoX = posX +
                      (atualAlvo.coluna +
                       (proximaAlvo.coluna - atualAlvo.coluna) * fracaoAlvo) *
                          larguraCelula +
                      larguraCelula / 2.0f;

        float alvoY = posY +
                      (atualAlvo.linha +
                       (proximaAlvo.linha - atualAlvo.linha) * fracaoAlvo) *
                          alturaCelula +
                      alturaCelula / 2.0f;

        int tipoEfeito = 0;

        if (torres[i].tipo == TORRE_GELO)
            tipoEfeito = 1;
        else if (torres[i].tipo == TORRE_TROVAO)
            tipoEfeito = 2;

        registrarEfeito(
            tipoEfeito,
            torres[i].nivel,
            (Vector2){torreX, torreY - 12.0f},
            (Vector2){alvoX, alvoY});

        inimigos[alvo].vida -= torres[i].dano;

        if (torres[i].tipo == TORRE_GELO)
        {
            aplicarLentidao(inimigos, alvo, quantidadeInimigos, 2.0f);
        }

        if (torres[i].tipo == TORRE_TROVAO)
        {
            int atingidos = 0;
            float distanciaMaxima = torres[i].alcance * 0.5f;

            for (int j = 0; j < quantidadeInimigos; j++)
            {
                if (j == alvo || !inimigos[j].ativo)
                    continue;

                int indice = (int)inimigos[j].progresso;

                if (indice < 0 ||
                    indice >= jogo->quantidadeCaminho - 1)
                    continue;

                float fracao = inimigos[j].progresso - indice;

                Posicao atual = jogo->caminho[indice];
                Posicao proxima = jogo->caminho[indice + 1];

                float inimigoX = posX +
                                 (atual.coluna +
                                  (proxima.coluna - atual.coluna) * fracao) *
                                     larguraCelula +
                                 larguraCelula / 2.0f;

                float inimigoY = posY +
                                 (atual.linha +
                                  (proxima.linha - atual.linha) * fracao) *
                                     alturaCelula +
                                 alturaCelula / 2.0f;

                float dx = inimigoX - alvoX;
                float dy = inimigoY - alvoY;
                float distancia = sqrtf(dx * dx + dy * dy);

                if (distancia <= distanciaMaxima)
                {
                    registrarEfeito(
                        2,
                        torres[i].nivel,
                        (Vector2){alvoX, alvoY},
                        (Vector2){inimigoX, inimigoY});

                    inimigos[j].vida -= torres[i].dano * 0.5f;
                    atingidos++;

                    if (inimigos[j].vida <= 0.0f)
                    {
                        inimigos[j].vida = 0.0f;
                        inimigos[j].ativo = 0;
                        adicionarMoedas(jogo, inimigos[j].recompensa);
                    }

                    if (atingidos >= 2)
                        break;
                }
            }
        }

        torres[i].tempoRecarga = torres[i].intervaloAtaque;

        if (inimigos[alvo].vida <= 0.0f)
        {
            inimigos[alvo].vida = 0.0f;
            inimigos[alvo].ativo = 0;

            adicionarMoedas(jogo, inimigos[alvo].recompensa);
        }
    }
}

void desenharTorres(Torre torres[], int quantidade,
                    int posX, int posY,
                    int larguraCelula, int alturaCelula)
{
    if (torres == NULL || larguraCelula <= 0 || alturaCelula <= 0)
        return;

    for (int i = 0; i < quantidade; i++)
    {
        if (!torres[i].ativo)
            continue;

        float x = posX + torres[i].coluna * larguraCelula +
                  larguraCelula / 2.0f;

        float y = posY + torres[i].linha * alturaCelula +
                  alturaCelula / 2.0f;

        switch (torres[i].tipo)
        {
        case TORRE_ARQUEIRO:
            desenharTorreArqueiro(x, y, torres[i].nivel);
            break;

        case TORRE_GELO:
            desenharTorreGelo(x, y, torres[i].nivel);
            break;

        case TORRE_TROVAO:
            desenharTorreTrovao(x, y, torres[i].nivel);
            break;

        default:
            break;
        }
    }

    desenharEfeitosTorre();
}

int melhorarTorre(Torre torres[], int quantidade,
                  Jogo *jogo, int indice)
{
    if (torres == NULL || jogo == NULL)
        return 0;

    if (indice < 0 || indice >= quantidade)
        return 0;

    Torre *torre = &torres[indice];

    if (!torre->ativo || torre->nivel >= 3)
        return 0;

    int custoMelhoria = torre->custo * torre->nivel;

    if (jogo->moedas < custoMelhoria)
        return 0;

    jogo->moedas -= custoMelhoria;
    torre->valorInvestido += custoMelhoria;

    torre->dano = (torre->dano * 3 + 1) / 2;
    torre->alcance *= 1.10f;
    torre->intervaloAtaque *= 0.90f;
    torre->nivel++;

    return 1;
}

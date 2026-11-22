
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "jogo.h"
#include "mapa.h"
#include "inimigo.h"
#include "visual.h"
#include "onda.h"
#include "torre.h"
#include "persistencia.h"

#define LARGURA_TELA 1280
#define ALTURA_TELA 720
#define FPS 60

#define LARGURA_CELULA 64
#define ALTURA_CELULA 52
#define ORIGEM_X 0
#define ORIGEM_Y 100

typedef enum
{
    MENU_INICIAL,
    MENU_INSTRUCOES,
    MENU_RECORDE,
    EM_JOGO,
    JOGO_PAUSADO
} EstadoTela;

static Font fonteTitulo;
static Font fonteInterface;

static int fonteTituloCarregada = 0;
static int fonteInterfaceCarregada = 0;
static char mensagemPausa[160] = "A partida esta pausada.";

static const Color COR_DOURADO = {236, 190, 91, 255};
static const Color COR_PELE = {224, 178, 137, 255};
static const Color COR_BORDA = {117, 91, 53, 255};
static const Color COR_TEXTO = {245, 239, 218, 255};
static const Color COR_ROXO = {151, 91, 210, 255};

static void desenharTextoUI(const char *texto, float x, float y, float tamanho, Color cor)
{
    DrawTextEx(fonteInterface, texto, (Vector2){x, y}, tamanho, 1.0f, cor);
}

static float desenharTextoAjustado(const char *texto, float x, float y, float larguraMaxima,
                                   float tamanhoMaximo, float tamanhoMinimo, Color cor)
{
    float tamanho = tamanhoMaximo;
    Vector2 dimensoes = MeasureTextEx(fonteInterface, texto, tamanho, 1.0f);

    while (dimensoes.x > larguraMaxima && tamanho > tamanhoMinimo)
    {
        tamanho -= 0.5f;
        dimensoes = MeasureTextEx(fonteInterface, texto, tamanho, 1.0f);
    }

    DrawTextEx(fonteInterface, texto, (Vector2){x, y}, tamanho, 1.0f, cor);
    return dimensoes.x;
}

static void desenharTextoTitulo(const char *texto, float x, float y, float tamanho, Color cor)
{
    DrawTextEx(fonteTitulo, texto, (Vector2){x, y}, tamanho, 1.0f, cor);
}

static void desenharPainel(Rectangle area, Color fundo)
{
    DrawRectangleRounded(area, 0.12f, 8, fundo);
    DrawRectangleRoundedLinesEx(area, 0.12f, 8, 2, COR_BORDA);

    DrawLineEx(
        (Vector2){area.x + 12, area.y + 3},
        (Vector2){area.x + area.width - 12, area.y + 3},
        1,
        (Color){225, 185, 105, 100});
}

static void desenharTextoCentralizado(const char *texto, Font fonte, float tamanho, float y, Color cor)
{
    Vector2 dimensoes = MeasureTextEx(fonte, texto, tamanho, 1);

    DrawTextEx(
        fonte,
        texto,
        (Vector2){(LARGURA_TELA - dimensoes.x) / 2.0f, y},
        tamanho,
        1,
        cor);
}

static void desenharContorno(Rectangle area, Color cor, float espessura)
{
    DrawRectangleRoundedLinesEx(area, 0.15f, 8, espessura, cor);
}

static int desenharBotao(Rectangle botao, const char *texto, int destaque)
{
    Vector2 mouse = GetMousePosition();
    int sobre = CheckCollisionPointRec(mouse, botao);
    float tempo = (float)GetTime();

    Color fundo = destaque
                      ? (Color){65, 112, 78, 255}
                      : (Color){39, 67, 76, 255};

    Color borda = destaque
                      ? (Color){189, 229, 132, 255}
                      : (Color){125, 177, 184, 255};

    Color corTexto = RAYWHITE;

    if (sobre)
    {
        fundo = destaque
                    ? (Color){88, 151, 94, 255}
                    : (Color){55, 101, 112, 255};

        borda = (Color){255, 229, 139, 255};
    }

    if (destaque)
    {
        DrawRectangleRounded(
            (Rectangle){botao.x - 4, botao.y - 4, botao.width + 8, botao.height + 8},
            0.2f,
            8,
            (Color){255, 218, 111, sobre ? 75 : 35});
    }

    DrawRectangleRounded(
        (Rectangle){botao.x + 4, botao.y + 5, botao.width, botao.height},
        0.18f,
        8,
        (Color){5, 15, 20, 130});

    DrawRectangleRounded(botao, 0.18f, 8, fundo);
    desenharContorno(botao, borda, sobre ? 4 : 3);

    DrawLineEx(
        (Vector2){botao.x + 15, botao.y + 7},
        (Vector2){botao.x + botao.width - 15, botao.y + 7},
        2,
        (Color){255, 255, 255, 75});

    if (destaque)
    {
        float brilho = 0.5f + sinf(tempo * 3.0f) * 0.5f;

        DrawCircle(
            (int)(botao.x + 22),
            (int)(botao.y + botao.height / 2),
            4 + brilho * 2,
            (Color){255, 229, 137, 220});
    }

    float tamanhoFonte = botao.height >= 64 ? 24.0f : botao.height >= 50 ? 21.0f
                                                  : botao.height >= 40   ? 18.0f
                                                                         : 16.0f;
    Vector2 tamanhoTexto = MeasureTextEx(fonteInterface, texto, tamanhoFonte, 1.0f);

    while (tamanhoTexto.x > botao.width - 24 && tamanhoFonte > 14.0f)
    {
        tamanhoFonte -= 0.5f;
        tamanhoTexto = MeasureTextEx(fonteInterface, texto, tamanhoFonte, 1.0f);
    }

    DrawTextEx(
        fonteInterface,
        texto,
        (Vector2){
            botao.x + (botao.width - tamanhoTexto.x) / 2,
            botao.y + (botao.height - tamanhoTexto.y) / 2},
        tamanhoFonte,
        1.0f,
        corTexto);

    return sobre && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

static void desenharNuvem(float x, float y, float escala)
{
    Color nuvem = (Color){196, 218, 211, 190};

    DrawEllipse((int)x, (int)y, 48 * escala, 17 * escala, nuvem);
    DrawCircle((int)(x - 23 * escala), (int)(y - 7 * escala), 17 * escala, nuvem);
    DrawCircle((int)x, (int)(y - 14 * escala), 23 * escala, nuvem);
    DrawCircle((int)(x + 25 * escala), (int)(y - 5 * escala), 16 * escala, nuvem);
}

static void desenharMontanhas(void)
{
    DrawTriangle(
        (Vector2){0, 380},
        (Vector2){210, 120},
        (Vector2){450, 380},
        (Color){47, 82, 96, 255});

    DrawTriangle(
        (Vector2){230, 380},
        (Vector2){520, 90},
        (Vector2){850, 380},
        (Color){57, 99, 105, 255});

    DrawTriangle(
        (Vector2){620, 380},
        (Vector2){940, 115},
        (Vector2){1280, 380},
        (Color){42, 78, 91, 255});

    DrawTriangle(
        (Vector2){210, 120},
        (Vector2){165, 185},
        (Vector2){275, 175},
        (Color){215, 230, 222, 255});

    DrawTriangle(
        (Vector2){520, 90},
        (Vector2){455, 170},
        (Vector2){590, 160},
        (Color){225, 236, 226, 255});

    DrawTriangle(
        (Vector2){940, 115},
        (Vector2){870, 185},
        (Vector2){1010, 180},
        (Color){218, 231, 224, 255});

    DrawTriangle(
        (Vector2){0, 380},
        (Vector2){210, 120},
        (Vector2){450, 380},
        (Color){255, 255, 255, 18});

    DrawTriangle(
        (Vector2){230, 380},
        (Vector2){520, 90},
        (Vector2){850, 380},
        (Color){255, 255, 255, 15});
}

static void desenharArvoreMenu(float x, float y, float escala, Color cor)
{
    DrawRectangle(
        (int)(x - 8 * escala),
        (int)y,
        (int)(16 * escala),
        (int)(65 * escala),
        (Color){91, 62, 40, 255});

    DrawTriangle(
        (Vector2){x, y - 100 * escala},
        (Vector2){x - 48 * escala, y - 12 * escala},
        (Vector2){x + 48 * escala, y - 12 * escala},
        cor);

    DrawTriangle(
        (Vector2){x, y - 70 * escala},
        (Vector2){x - 58 * escala, y + 22 * escala},
        (Vector2){x + 58 * escala, y + 22 * escala},
        (Color){cor.r > 20 ? cor.r - 8 : cor.r, cor.g > 15 ? cor.g - 12 : cor.g, cor.b, 255});

    DrawCircle(
        (int)(x - 15 * escala),
        (int)(y - 35 * escala),
        5 * escala,
        (Color){116, 168, 99, 255});
}

static void desenharYggdrasil(void)
{
    float tempo = (float)GetTime();
    float pulso = sinf(tempo * 2.0f) * 4.0f;

    DrawCircle(640, 220, 180 + pulso, (Color){68, 166, 113, 25});
    DrawCircle(640, 220, 145 + pulso, (Color){76, 190, 117, 22});

    DrawLineEx(
        (Vector2){640, 390},
        (Vector2){640, 115},
        45,
        (Color){100, 75, 45, 255});

    DrawLineEx(
        (Vector2){640, 300},
        (Vector2){460, 125},
        25,
        (Color){111, 81, 47, 255});

    DrawLineEx(
        (Vector2){640, 270},
        (Vector2){810, 110},
        25,
        (Color){111, 81, 47, 255});

    DrawLineEx(
        (Vector2){640, 235},
        (Vector2){420, 205},
        18,
        (Color){111, 81, 47, 255});

    DrawLineEx(
        (Vector2){640, 235},
        (Vector2){860, 195},
        18,
        (Color){111, 81, 47, 255});

    DrawCircle(640, 145, 104, (Color){31, 112, 69, 255});
    DrawCircle(555, 180, 77, (Color){38, 132, 75, 255});
    DrawCircle(720, 170, 83, (Color){35, 122, 68, 255});
    DrawCircle(640, 75, 66, (Color){48, 143, 77, 255});
    DrawCircle(485, 170, 45, (Color){43, 119, 72, 255});
    DrawCircle(790, 155, 48, (Color){43, 119, 72, 255});

    DrawCircle(640, 145, 12 + pulso * 0.4f, (Color){255, 221, 113, 255});
    DrawCircle(640, 145, 5, WHITE);

    for (int i = 0; i < 18; i++)
    {
        float angulo = tempo * 0.4f + i * 0.75f;
        float raio = 85 + (i % 4) * 18;
        float x = 640 + cosf(angulo) * raio;
        float y = 180 + sinf(angulo) * raio * 0.65f;

        DrawCircle((int)x, (int)y, 2 + (i % 3), (Color){180, 242, 150, 190});
    }
}

static void desenharCenarioMenu(void)
{
    float tempo = (float)GetTime();

    ClearBackground((Color){93, 166, 177, 255});

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, ALTURA_TELA,
        (Color){87, 160, 181, 255},
        (Color){192, 215, 176, 255});

    DrawCircle(1030, 115, 72, (Color){255, 229, 153, 75});
    DrawCircle(1030, 115, 45, (Color){255, 230, 164, 255});

    desenharNuvem(160 + sinf(tempo * 0.15f) * 18, 145, 0.9f);
    desenharNuvem(390 + sinf(tempo * 0.12f) * 12, 90, 0.65f);
    desenharNuvem(1090 + sinf(tempo * 0.16f) * 15, 210, 0.8f);

    desenharMontanhas();

    DrawRectangleGradientV(
        0, 350,
        LARGURA_TELA, 370,
        (Color){77, 146, 87, 255},
        (Color){34, 89, 57, 255});

    DrawEllipse(640, 460, 390, 115, (Color){115, 172, 96, 255});
    DrawEllipse(640, 485, 310, 75, (Color){139, 187, 103, 255});

    desenharYggdrasil();

    desenharArvoreMenu(55, 430, 1.45f, (Color){29, 92, 57, 255});
    desenharArvoreMenu(170, 450, 1.0f, (Color){35, 111, 62, 255});
    desenharArvoreMenu(1100, 430, 1.45f, (Color){29, 92, 57, 255});
    desenharArvoreMenu(1210, 450, 1.0f, (Color){35, 111, 62, 255});

    for (int i = 0; i < 35; i++)
    {
        float x = (float)((i * 137) % LARGURA_TELA);
        float y = 375 + (float)((i * 43) % 260);
        float movimento = sinf(tempo * 1.5f + i) * 5.0f;

        DrawCircle((int)(x + movimento), (int)y, 2 + (i % 3), (Color){210, 236, 145, 190});
    }

    for (int i = 0; i < 12; i++)
    {
        float x = (float)((i * 113) % LARGURA_TELA);
        float y = 580 + sinf(tempo * 1.8f + i) * 13;

        DrawEllipse((int)x, (int)y, 4, 8, (Color){177, 211, 104, 220});
    }

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, 190,
        (Color){8, 28, 35, 95},
        (Color){8, 28, 35, 0});
}

static void desenharHeroi(
    int x,
    int y,
    Color armadura,
    Color detalhe,
    int tipoArma)
{
    float tempo = (float)GetTime();
    float movimento = sinf(tempo * 2.0f + x) * 2.0f;

    y += (int)movimento;

    DrawEllipse(x, y + 112, 39, 9, (Color){15, 36, 27, 90});

    DrawRectangleRounded(
        (Rectangle){x - 18, y + 59, 15, 45},
        0.25f,
        5,
        (Color){46, 49, 46, 255});

    DrawRectangleRounded(
        (Rectangle){x + 3, y + 59, 15, 45},
        0.25f,
        5,
        (Color){46, 49, 46, 255});

    DrawRectangleRounded(
        (Rectangle){x - 24, y + 96, 25, 13},
        0.3f,
        5,
        (Color){56, 42, 31, 255});

    DrawRectangleRounded(
        (Rectangle){x + 1, y + 96, 25, 13},
        0.3f,
        5,
        (Color){56, 42, 31, 255});

    DrawTriangle(
        (Vector2){x - 25, y + 8},
        (Vector2){x - 46, y + 88},
        (Vector2){x + 3, y + 80},
        (Color){armadura.r / 2, armadura.g / 2, armadura.b / 2, 255});

    DrawTriangle(
        (Vector2){x + 25, y + 8},
        (Vector2){x + 46, y + 88},
        (Vector2){x - 3, y + 80},
        (Color){armadura.r / 2, armadura.g / 2, armadura.b / 2, 255});

    DrawRectangleRounded(
        (Rectangle){x - 29, y + 3, 58, 68},
        0.2f,
        6,
        armadura);

    DrawRectangle(
        x - 7,
        y + 7,
        14,
        59,
        detalhe);

    DrawRectangleRounded(
        (Rectangle){x - 29, y + 35, 58, 9},
        0.3f,
        5,
        (Color){126, 91, 49, 255});

    DrawCircle(x, y + 39, 6, COR_DOURADO);

    DrawCircle(x - 29, y + 16, 12, detalhe);
    DrawCircle(x + 29, y + 16, 12, detalhe);

    DrawRectangle(x - 9, y - 12, 18, 23, COR_PELE);
    DrawCircle(x, y - 27, 23, COR_PELE);

    DrawCircle(x, y - 41, 23, (Color){72, 48, 34, 255});
    DrawRectangle(x - 23, y - 40, 46, 15, (Color){72, 48, 34, 255});

    DrawCircle(x - 8, y - 27, 3, BLACK);
    DrawCircle(x + 8, y - 27, 3, BLACK);

    DrawLineEx(
        (Vector2){x - 6, y - 17},
        (Vector2){x + 6, y - 17},
        2,
        (Color){119, 58, 47, 255});

    if (tipoArma == 0)
    {
        DrawLineEx(
            (Vector2){x + 37, y + 15},
            (Vector2){x + 65, y - 48},
            7,
            (Color){106, 75, 43, 255});

        DrawLineEx(
            (Vector2){x + 65, y - 48},
            (Vector2){x + 80, y - 65},
            5,
            (Color){220, 229, 224, 255});

        DrawLineEx(
            (Vector2){x + 27, y + 17},
            (Vector2){x + 47, y + 24},
            8,
            (Color){130, 91, 48, 255});
    }
    else if (tipoArma == 1)
    {
        DrawLineEx(
            (Vector2){x - 43, y + 15},
            (Vector2){x - 68, y - 55},
            7,
            (Color){119, 79, 42, 255});

        DrawTriangle(
            (Vector2){x - 68, y - 55},
            (Vector2){x - 91, y - 67},
            (Vector2){x - 76, y - 29},
            (Color){222, 229, 220, 255});

        DrawLineEx(
            (Vector2){x - 68, y - 55},
            (Vector2){x - 76, y - 29},
            2,
            WHITE);
    }
    else if (tipoArma == 2)
    {
        DrawLineEx(
            (Vector2){x + 38, y + 75},
            (Vector2){x + 38, y - 65},
            8,
            (Color){105, 76, 45, 255});

        DrawCircle(x + 38, y - 69, 15, (Color){75, 166, 213, 255});
        DrawCircle(x + 38, y - 69, 8, (Color){183, 239, 255, 255});
        DrawCircle(x + 38, y - 69, 3, WHITE);
    }
    else
    {
        DrawLineEx(
            (Vector2){x - 40, y - 45},
            (Vector2){x - 58, y + 40},
            7,
            (Color){119, 79, 42, 255});

        DrawLineEx(
            (Vector2){x - 58, y + 40},
            (Vector2){x - 40, y + 105},
            7,
            (Color){119, 79, 42, 255});

        DrawLineEx(
            (Vector2){x - 40, y - 45},
            (Vector2){x - 40, y + 105},
            2,
            (Color){221, 226, 215, 255});
    }
}

static void desenharMonstroDraugr(int x, int y)
{
    Color pele = (Color){105, 143, 100, 255};
    Color roupa = (Color){54, 77, 62, 255};

    DrawEllipse(x, y + 45, 35, 8, (Color){15, 35, 25, 100});

    DrawRectangleRounded(
        (Rectangle){x - 25, y + 10, 50, 62},
        0.2f,
        5,
        roupa);

    DrawCircle(x, y - 12, 27, pele);
    DrawCircle(x - 9, y - 15, 5, (Color){255, 79, 65, 255});
    DrawCircle(x + 9, y - 15, 5, (Color){255, 79, 65, 255});

    DrawRectangle(x - 13, y - 2, 26, 5, (Color){44, 48, 39, 255});

    DrawRectangleRounded(
        (Rectangle){x - 35, y + 17, 12, 39},
        0.3f,
        5,
        (Color){78, 92, 68, 255});

    DrawRectangleRounded(
        (Rectangle){x + 23, y + 17, 12, 39},
        0.3f,
        5,
        (Color){78, 92, 68, 255});

    DrawLineEx(
        (Vector2){x + 36, y + 30},
        (Vector2){x + 58, y - 25},
        5,
        (Color){112, 80, 49, 255});

    DrawLineEx(
        (Vector2){x + 58, y - 25},
        (Vector2){x + 68, y - 39},
        4,
        LIGHTGRAY);
}

static void desenharMonstroFenrir(int x, int y)
{
    Color pelo = (Color){83, 91, 89, 255};
    Color sombra = (Color){47, 58, 58, 255};

    DrawEllipse(x, y + 35, 49, 12, (Color){15, 35, 25, 100});

    DrawEllipse(x, y + 5, 47, 27, pelo);
    DrawCircle(x + 28, y - 13, 28, pelo);

    DrawTriangle(
        (Vector2){x + 10, y - 30},
        (Vector2){x + 5, y - 68},
        (Vector2){x + 35, y - 38},
        pelo);

    DrawTriangle(
        (Vector2){x + 43, y - 30},
        (Vector2){x + 62, y - 63},
        (Vector2){x + 61, y - 23},
        pelo);

    DrawEllipse(x + 50, y - 2, 24, 13, sombra);

    DrawCircle(x + 22, y - 17, 5, (Color){255, 221, 94, 255});
    DrawCircle(x + 43, y - 17, 5, (Color){255, 221, 94, 255});

    DrawCircle(x + 22, y - 17, 2, BLACK);
    DrawCircle(x + 43, y - 17, 2, BLACK);

    DrawTriangle(
        (Vector2){x + 68, y - 4},
        (Vector2){x + 55, y - 10},
        (Vector2){x + 56, y + 5},
        (Color){35, 39, 39, 255});

    DrawLineEx(
        (Vector2){x - 25, y + 10},
        (Vector2){x - 43, y + 45},
        10,
        pelo);

    DrawLineEx(
        (Vector2){x + 5, y + 15},
        (Vector2){x - 1, y + 46},
        10,
        pelo);

    DrawLineEx(
        (Vector2){x + 27, y + 14},
        (Vector2){x + 34, y + 45},
        10,
        pelo);
}

static void desenharMonstroTroll(int x, int y)
{
    Color pele = (Color){113, 139, 91, 255};
    Color sombra = (Color){75, 103, 72, 255};

    DrawEllipse(x, y + 65, 55, 13, (Color){15, 35, 25, 100});

    DrawRectangleRounded(
        (Rectangle){x - 44, y - 5, 88, 78},
        0.3f,
        7,
        sombra);

    DrawCircle(x, y - 24, 40, pele);

    DrawTriangle(
        (Vector2){x - 23, y - 48},
        (Vector2){x - 50, y - 83},
        (Vector2){x - 5, y - 59},
        (Color){184, 183, 143, 255});

    DrawTriangle(
        (Vector2){x + 23, y - 48},
        (Vector2){x + 50, y - 83},
        (Vector2){x + 5, y - 59},
        (Color){184, 183, 143, 255});

    DrawCircle(x - 14, y - 30, 6, (Color){255, 220, 93, 255});
    DrawCircle(x + 14, y - 30, 6, (Color){255, 220, 93, 255});

    DrawRectangleRounded(
        (Rectangle){x - 17, y - 7, 34, 13},
        0.3f,
        4,
        (Color){65, 70, 54, 255});

    DrawRectangleRounded(
        (Rectangle){x - 62, y + 9, 20, 47},
        0.4f,
        5,
        pele);

    DrawRectangleRounded(
        (Rectangle){x + 42, y + 9, 20, 47},
        0.4f,
        5,
        pele);

    DrawCircle(x - 52, y + 57, 12, pele);
    DrawCircle(x + 52, y + 57, 12, pele);

    DrawCircle(x - 28, y + 5, 9, (Color){150, 163, 113, 255});
    DrawCircle(x + 26, y + 28, 10, (Color){150, 163, 113, 255});
}

static void desenharMonstroHel(int x, int y)
{
    Color armadura = (Color){68, 79, 105, 255};
    Color energia = (Color){121, 204, 232, 255};

    DrawEllipse(x, y + 49, 38, 10, (Color){15, 35, 25, 100});

    DrawRectangleRounded(
        (Rectangle){x - 30, y + 8, 60, 66},
        0.2f,
        6,
        armadura);

    DrawCircle(x, y - 15, 29, armadura);

    DrawTriangle(
        (Vector2){x - 20, y - 31},
        (Vector2){x - 43, y - 77},
        (Vector2){x - 5, y - 44},
        armadura);

    DrawTriangle(
        (Vector2){x + 20, y - 31},
        (Vector2){x + 43, y - 77},
        (Vector2){x + 5, y - 44},
        armadura);

    DrawCircle(x - 10, y - 17, 5, energia);
    DrawCircle(x + 10, y - 17, 5, energia);

    DrawRectangle(x - 12, y + 2, 24, 6, (Color){35, 48, 66, 255});

    DrawLineEx(
        (Vector2){x + 35, y + 20},
        (Vector2){x + 50, y - 44},
        5,
        (Color){111, 93, 72, 255});

    DrawTriangle(
        (Vector2){x + 50, y - 44},
        (Vector2){x + 42, y - 63},
        (Vector2){x + 60, y - 58},
        energia);
}

static void desenharHeroisMenu(void)
{
    desenharHeroi(
        390, 420,
        (Color){87, 112, 119, 255},
        (Color){191, 153, 88, 255},
        0);

    desenharHeroi(
        520, 420,
        (Color){92, 108, 73, 255},
        (Color){191, 169, 105, 255},
        1);

    desenharHeroi(
        650, 420,
        (Color){65, 91, 132, 255},
        (Color){125, 190, 224, 255},
        2);

    desenharHeroi(
        780, 420,
        (Color){107, 75, 67, 255},
        (Color){207, 153, 89, 255},
        3);
}

static void desenharCriaturasMenu(void)
{
    desenharMonstroDraugr(280, 400);
    desenharMonstroFenrir(930, 400);
    desenharMonstroTroll(1080, 370);
    desenharMonstroHel(170, 390);
}

static void desenharMenuInicialDecoracao(void)
{
    float tempo = (float)GetTime();

    desenharCenarioMenu();
    desenharCriaturasMenu();
    desenharHeroisMenu();

    for (int i = 0; i < 24; i++)
    {
        float x = (float)((i * 89) % LARGURA_TELA);
        float y = 190.0f + (float)((i * 47) % 390);
        float deslocamento = sinf(tempo * 1.7f + i) * 8.0f;

        DrawCircle(
            (int)(x + deslocamento),
            (int)y,
            2 + (i % 2),
            (Color){255, 231, 143, 190});
    }

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, 175,
        (Color){9, 26, 34, 170},
        (Color){9, 26, 34, 0});

    DrawRectangleGradientV(
        0, 490,
        LARGURA_TELA, 230,
        (Color){8, 25, 24, 0},
        (Color){7, 20, 21, 220});
}

static int desenharMenuInicial(void)
{
    Rectangle botaoJogar = {475, 515, 330, 54};
    Rectangle botaoRecordes = {500, 578, 280, 42};
    Rectangle botaoInstrucoes = {500, 628, 280, 38};
    Rectangle botaoSair = {500, 674, 280, 34};

    desenharMenuInicialDecoracao();

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, 175,
        (Color){8, 24, 31, 185},
        (Color){8, 24, 31, 0});

    desenharTextoCentralizado(
        "RAGNAROK",
        fonteTitulo,
        76,
        9,
        (Color){255, 225, 137, 255});

    Vector2 titulo = MeasureTextEx(fonteTitulo, "RAGNAROK", 76, 1);

    DrawTextEx(
        fonteTitulo,
        "RAGNAROK",
        (Vector2){(LARGURA_TELA - titulo.x) / 2.0f + 3, 12},
        76,
        1,
        (Color){77, 48, 27, 170});

    desenharTextoCentralizado(
        "ULTIMA DEFESA",
        fonteInterface,
        29,
        87,
        RAYWHITE);

    desenharTextoCentralizado(
        "QUATRO HEROIS. UM ULTIMO DESTINO.",
        fonteInterface,
        18,
        133,
        (Color){255, 232, 180, 255});

    DrawLineEx(
        (Vector2){420, 165},
        (Vector2){860, 165},
        3,
        (Color){236, 190, 91, 210});

    DrawCircle(420, 165, 5, (Color){255, 225, 137, 255});
    DrawCircle(860, 165, 5, (Color){255, 225, 137, 255});

    if (desenharBotao(botaoJogar, "JOGAR", 1))
        return 1;

    if (desenharBotao(botaoRecordes, "RECORDES", 0))
        return 4;

    if (desenharBotao(botaoInstrucoes, "COMO JOGAR", 0))
        return 2;

    if (desenharBotao(botaoSair, "SAIR", 0))
        return 3;

    return 0;
}

static int desenharTelaRecordes(void)
{
    Recorde recordes[MAX_RECORDES] = {0};
    Estatisticas estatisticas = {0};
    int quantidade = carregarRecordes(recordes, MAX_RECORDES);
    carregarEstatisticas(&estatisticas);

    ClearBackground((Color){12, 23, 29, 255});
    DrawRectangleGradientV(0, 0, LARGURA_TELA, ALTURA_TELA,
                           (Color){23, 43, 47, 255}, (Color){8, 17, 21, 255});
    DrawRectangle(0, 0, LARGURA_TELA, 92, (Color){18, 35, 39, 255});
    DrawLine(0, 91, LARGURA_TELA, 91, GOLD);
    desenharTextoCentralizado("HALL DA GLORIA DE ASGARD", fonteTitulo, 39, 22, GOLD);

    desenharPainel((Rectangle){55, 112, 750, 500}, (Color){13, 25, 29, 240});
    desenharTextoUI("RANKING DOS 10 MELHORES", 82, 130, 23, GOLD);

    desenharTextoUI("POS", 82, 176, 16, (Color){185, 205, 195, 255});
    desenharTextoUI("JOGADOR", 130, 176, 16, (Color){185, 205, 195, 255});
    desenharTextoUI("PONTOS", 315, 176, 16, (Color){185, 205, 195, 255});
    desenharTextoUI("ONDA", 435, 176, 16, (Color){185, 205, 195, 255});
    desenharTextoUI("RESULTADO", 505, 176, 16, (Color){185, 205, 195, 255});
    desenharTextoUI("DATA", 650, 176, 16, (Color){185, 205, 195, 255});
    DrawLine(78, 202, 782, 202, (Color){117, 91, 53, 180});

    if (quantidade == 0)
    {
        desenharTextoAjustado("Nenhuma partida registrada ainda.", 100, 250, 620, 22, 18, COR_TEXTO);
        desenharTextoAjustado("Jogue para conquistar seu lugar no Hall da Gloria!", 100, 292, 620, 18, 15, (Color){190, 205, 195, 255});
    }
    else
    {
        for (int i = 0; i < quantidade; i++)
        {
            float y = 216.0f + i * 34.0f;
            Color cor = i == 0 ? GOLD : i == 1 ? (Color){205, 215, 220, 255}
                                    : i == 2   ? (Color){190, 135, 90, 255}
                                               : COR_TEXTO;
            const char *resultado = recordes[i].vitoria ? "VITORIA" : "DERROTA";

            desenharTextoUI(TextFormat("%02d", i + 1), 84, y, 18, cor);
            desenharTextoAjustado(recordes[i].nome, 130, y, 170, 18, 13, cor);
            desenharTextoUI(TextFormat("%d", recordes[i].pontuacao), 315, y, 18, COR_TEXTO);
            desenharTextoUI(TextFormat("%d/10", recordes[i].onda), 435, y, 18, COR_TEXTO);
            desenharTextoUI(resultado, 505, y, 17, recordes[i].vitoria ? (Color){170, 225, 150, 255} : (Color){225, 135, 125, 255});
            desenharTextoAjustado(recordes[i].data, 650, y, 125, 16, 12, (Color){190, 205, 195, 255});
        }
    }

    desenharPainel((Rectangle){830, 112, 395, 500}, (Color){13, 25, 29, 240});
    desenharTextoUI("ESTATISTICAS GERAIS", 855, 130, 20, GOLD);
    DrawLine(855, 166, 1200, 166, (Color){117, 91, 53, 150});

    desenharTextoUI(TextFormat("Partidas jogadas: %d", estatisticas.partidas), 855, 185, 19, COR_TEXTO);
    desenharTextoUI(TextFormat("Vitorias: %d", estatisticas.vitorias), 855, 229, 19, (Color){170, 225, 150, 255});
    desenharTextoUI(TextFormat("Derrotas: %d", estatisticas.derrotas), 855, 273, 19, (Color){225, 135, 125, 255});
    desenharTextoUI(TextFormat("Melhor onda: %d/10", estatisticas.melhorOnda), 855, 317, 19, COR_TEXTO);
    desenharTextoAjustado(TextFormat("Maior pontuacao: %d", estatisticas.maiorPontuacao), 855, 361, 345, 19, 15, COR_TEXTO);

    DrawLine(855, 410, 1200, 410, (Color){117, 91, 53, 150});
    desenharTextoUI("COMO A PONTUACAO E CALCULADA", 855, 430, 16, GOLD);
    desenharTextoAjustado("Onda alcancada + vida da base + moedas", 855, 462, 345, 16, 13, COR_TEXTO);
    desenharTextoAjustado("Bonus de 5.000 pontos por vitoria", 855, 489, 345, 16, 13, COR_TEXTO);

    return desenharBotao((Rectangle){490, 635, 300, 58}, "VOLTAR AO MENU", 1);
}

static int desenharTelaNomeJogador(char nome[], int capacidade, int *campoSelecionado)
{
    Rectangle campo = {350, 365, 580, 66};
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        *campoSelecionado = CheckCollisionPointRec(mouse, campo);

    if (*campoSelecionado)
    {
        int caractere = GetCharPressed();
        while (caractere > 0)
        {
            int tamanho = (int)strlen(nome);
            if (caractere >= 32 && caractere <= 126 && tamanho < capacidade - 1)
            {
                nome[tamanho] = (char)caractere;
                nome[tamanho + 1] = '\0';
            }
            caractere = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            int tamanho = (int)strlen(nome);
            if (tamanho > 0)
                nome[tamanho - 1] = '\0';
        }
    }

    desenharPainel((Rectangle){270, 215, 740, 290}, (Color){10, 20, 24, 250});
    desenharTextoCentralizado("REGISTRE SUA JORNADA", fonteTitulo, 40, 245, GOLD);
    desenharTextoCentralizado("Digite seu nome para salvar o resultado", fonteInterface, 23, 315, COR_TEXTO);

    DrawRectangleRounded(campo, 0.12f, 8, *campoSelecionado ? (Color){32, 49, 49, 255} : (Color){23, 34, 36, 255});
    DrawRectangleRoundedLinesEx(campo, 0.12f, 8, *campoSelecionado ? 3 : 2,
                                *campoSelecionado ? (Color){255, 218, 120, 255} : COR_BORDA);

    const char *texto = nome[0] == '\0' ? "Digite seu nome..." : nome;
    Color corTexto = nome[0] == '\0' ? (Color){145, 158, 151, 255} : RAYWHITE;
    Vector2 dimensoes = MeasureTextEx(fonteInterface, texto, 30, 1.2f);
    float x = campo.x + (campo.width - dimensoes.x) / 2.0f;
    float y = campo.y + (campo.height - dimensoes.y) / 2.0f;
    DrawTextEx(fonteInterface, texto, (Vector2){x, y}, 30, 1.2f, corTexto);

    if (*campoSelecionado && ((int)(GetTime() * 2.0) % 2 == 0))
    {
        float cursorX = nome[0] == '\0' ? x : x + dimensoes.x + 3.0f;
        DrawRectangle((int)cursorX, (int)(campo.y + 17), 3, 32, GOLD);
    }

    desenharTextoCentralizado("CLIQUE NO CAMPO PARA DIGITAR", fonteInterface, 16, 448, (Color){190, 205, 195, 255});
    desenharTextoCentralizado("ENTER: confirmar     BACKSPACE: apagar", fonteInterface, 18, 475, (Color){220, 210, 180, 255});
    return *campoSelecionado && IsKeyPressed(KEY_ENTER) && nome[0] != '\0';
}

static int paginaInstrucoes = 0;

static int desenharInstrucoes(void)
{
    ClearBackground((Color){12, 23, 29, 255});
    DrawRectangleGradientV(0, 0, LARGURA_TELA, ALTURA_TELA,
                           (Color){23, 43, 47, 255}, (Color){8, 17, 21, 255});
    DrawRectangle(0, 0, LARGURA_TELA, 105, (Color){18, 35, 39, 255});
    DrawLine(0, 104, LARGURA_TELA, 104, GOLD);
    desenharTextoCentralizado("COMO JOGAR", fonteTitulo, 48, 22, GOLD);

    Rectangle painel = {105, 125, 1070, 455};
    desenharPainel(painel, (Color){13, 25, 29, 242});
    desenharTextoUI(TextFormat("GUIA DO JOGADOR  -  %d/3", paginaInstrucoes + 1), 140, 140, 19, (Color){190, 205, 195, 255});

    if (paginaInstrucoes == 0)
    {
        desenharTextoTitulo("1. OBJETIVO E ONDAS", 145, 180, 31, GOLD);
        desenharTextoUI("Defenda Yggdrasil: impeca que as criaturas atravessem o caminho e alcancem a base.", 150, 232, 21, RAYWHITE);
        desenharTextoUI("Cada inimigo que chega ao fim causa dano a vida da base.", 150, 266, 21, RAYWHITE);
        desenharTextoUI("A partida termina em derrota quando a vida de Yggdrasil chega a zero.", 150, 300, 21, (Color){235, 155, 145, 255});
        DrawLine(145, 345, 1135, 345, (Color){117, 91, 53, 170});
        desenharTextoTitulo("CAMPANHA: 10 ONDAS", 145, 365, 27, GOLD);
        desenharTextoUI("Pressione ESPACO para iniciar cada onda. Derrote todos os inimigos", 150, 408, 20, RAYWHITE);
        desenharTextoUI("para avancar. Vida, dano e velocidade aumentam a cada onda.", 150, 439, 20, RAYWHITE);
        desenharTextoUI("Nas ondas avancadas surgem Fenrir, Troll e Guerreiro de Hel.", 150, 474, 20, RAYWHITE);
        desenharTextoUI("Onda 9: inimigos corrompidos. Onda 10: Jormungandr, chefe final.", 150, 508, 20, COR_ROXO);
        desenharTextoUI("Dica: planeje o posicionamento das torres entre as ondas.", 150, 544, 18, (Color){220, 205, 160, 255});
    }
    else if (paginaInstrucoes == 1)
    {
        desenharTextoTitulo("2. CONSTRUIR, MELHORAR E VENDER", 145, 180, 29, GOLD);
        desenharTextoUI("1. Clique em um tipo de torre na barra inferior para seleciona-lo.", 150, 225, 20, RAYWHITE);
        desenharTextoUI("2. Clique em uma casa livre (terreno marcado como espaco vazio) para construir.", 150, 258, 20, RAYWHITE);
        desenharTextoUI("A construcao so ocorre em posicao valida e se voce tiver moedas suficientes.", 150, 291, 20, RAYWHITE);
        DrawLine(145, 328, 1135, 328, (Color){117, 91, 53, 170});
        desenharTextoTitulo("TIPOS DE TORRE", 145, 345, 25, GOLD);
        desenharTextoUI("ARQUEIRO - ataque convencional equilibrado. Custo: 50 moedas.", 150, 383, 19, (Color){235, 205, 150, 255});
        desenharTextoUI("GELO - causa dano e reduz a velocidade do alvo por 2 segundos. Custo: 65.", 150, 414, 19, (Color){150, 220, 250, 255});
        desenharTextoUI("TROVAO - dano eletrico; pode atingir ate 2 inimigos proximos do alvo", 150, 445, 19, (Color){205, 165, 245, 255});
        desenharTextoUI("adicionais, causando metade do dano. Custo: 90 moedas.", 150, 474, 19, (Color){205, 165, 245, 255});
        desenharTextoTitulo("MELHORIA E VENDA", 145, 505, 23, GOLD);
        desenharTextoUI("Melhorias ate nivel 3: arqueiro 50/100; gelo 65/130; trovao 90/180 moedas.", 150, 538, 17, RAYWHITE);
        desenharTextoUI("Vender devolve 50% de todo o investimento (construcao + melhorias).", 150, 562, 17, RAYWHITE);
    }
    else
    {
        desenharTextoTitulo("3. CONTROLES E DICAS", 145, 180, 31, GOLD);
        desenharTextoUI("BOTAO ESQUERDO - clique nos cartoes inferiores para escolher a torre.", 150, 232, 21, RAYWHITE);
        desenharTextoUI("BOTAO ESQUERDO - clique em casa livre para construir ou numa torre para seleciona-la.", 150, 270, 20, RAYWHITE);
        desenharTextoUI("BOTAO ESQUERDO - com uma torre selecionada, use MELHORAR ou VENDER no painel.", 150, 307, 20, RAYWHITE);
        desenharTextoUI("Para cancelar, escolha outro cartao ou clique numa casa vazia.", 150, 344, 19, RAYWHITE);
        desenharTextoUI("Esse clique tambem tenta construir no local escolhido.", 150, 375, 19, RAYWHITE);
        DrawLine(145, 408, 1135, 408, (Color){117, 91, 53, 170});
        desenharTextoUI("ESPACO - inicia a proxima onda quando a onda atual nao esta ativa.", 150, 427, 20, SKYBLUE);
        desenharTextoUI("PAUSAR - botao no canto superior direito; CONTINUAR retoma a partida.", 150, 462, 20, SKYBLUE);
        desenharTextoUI("ESC - retorna ao menu nas telas de instrucoes e recordes. Nao pausa a partida.", 150, 497, 18, SKYBLUE);
        desenharTextoUI("Na pausa: salvar, carregar, reiniciar do inicio ou voltar ao menu.", 150, 530, 19, RAYWHITE);
        desenharTextoUI("As torres nao podem ocupar o caminho dos inimigos.", 150, 558, 18, (Color){220, 205, 160, 255});
    }

    int voltar = desenharBotao((Rectangle){105, 610, 220, 55}, "VOLTAR", 0);
    if (desenharBotao((Rectangle){835, 610, 150, 55}, "ANTERIOR", 0) && paginaInstrucoes > 0)
        paginaInstrucoes--;
    if (desenharBotao((Rectangle){1000, 610, 175, 55}, paginaInstrucoes == 2 ? "REINICIAR" : "PROXIMA", 1))
        paginaInstrucoes = paginaInstrucoes == 2 ? 0 : paginaInstrucoes + 1;
    if (voltar)
        paginaInstrucoes = 0;
    return voltar;
}

static void desenharBarraIndicadora(float x, float y, float largura, float altura, float valor, Color cor)
{
    if (valor < 0.0f)
        valor = 0.0f;

    if (valor > 1.0f)
        valor = 1.0f;

    DrawRectangleRounded(
        (Rectangle){x, y, largura, altura},
        0.5f,
        6,
        (Color){35, 40, 39, 255});

    DrawRectangleRounded(
        (Rectangle){x, y, largura * valor, altura},
        0.5f,
        6,
        cor);
}

static void desenharHUD(Jogo *jogo)
{
    float porcentagemVida = jogo->vidaBase / 100.0f;

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, 105,
        (Color){12, 23, 26, 255},
        (Color){22, 35, 35, 250});

    DrawRectangle(0, 101, LARGURA_TELA, 4, (Color){112, 83, 45, 255});

    desenharTextoTitulo("RAGNAROK", 28, 7, 34, GOLD);
    desenharTextoUI("ULTIMA DEFESA", 34, 49, 18, COR_TEXTO);

    DrawLineEx((Vector2){280, 19}, (Vector2){280, 83}, 1, (Color){135, 112, 75, 130});

    desenharTextoUI("MOEDAS", 320, 13, 19, (Color){182, 182, 168, 255});

    DrawCircle(329, 58, 12, (Color){120, 85, 34, 255});
    DrawCircle(329, 58, 8, GOLD);
    DrawCircle(326, 55, 2, (Color){255, 232, 165, 255});

    desenharTextoTitulo(TextFormat("%d", jogo->moedas), 348, 34, 29, GOLD);

    DrawLineEx((Vector2){510, 19}, (Vector2){510, 83}, 1, (Color){135, 112, 75, 130});

    desenharTextoUI("YGGDRASIL", 550, 13, 19, (Color){182, 182, 168, 255});

    desenharTextoTitulo(
        TextFormat("%d / 100", jogo->vidaBase),
        550, 34, 27,
        (Color){150, 218, 151, 255});

    desenharBarraIndicadora(
        550, 75, 155, 7,
        porcentagemVida,
        porcentagemVida > 0.3f
            ? (Color){91, 180, 111, 255}
            : (Color){210, 65, 62, 255});

    DrawLineEx((Vector2){750, 19}, (Vector2){750, 83}, 1, (Color){135, 112, 75, 130});

    desenharTextoUI("ONDA ATUAL", 790, 13, 20, (Color){182, 182, 168, 255});

    desenharTextoTitulo(
        TextFormat("%02d / 10", jogo->onda),
        790, 34, 29,
        SKYBLUE);

    DrawLineEx((Vector2){965, 19}, (Vector2){965, 83}, 1, (Color){135, 112, 75, 130});

    desenharBotao((Rectangle){1080, 20, 175, 65}, "PAUSAR", 0);
}

static void desenharMensagem(const char *mensagem)
{
    Vector2 dimensoes = MeasureTextEx(fonteInterface, mensagem, 21, 1);
    float largura = dimensoes.x;

    DrawRectangleRounded(
        (Rectangle){
            LARGURA_TELA / 2.0f - largura / 2.0f - 25,
            635,
            largura + 50,
            48},
        0.3f,
        8,
        (Color){16, 25, 24, 230});

    desenharTextoUI(
        mensagem,
        LARGURA_TELA / 2.0f - largura / 2.0f,
        644,
        21,
        RAYWHITE);
}

static void desenharIconeTorre(TipoTorre tipo, float x, float y, Color cor)
{
    if (tipo == TORRE_ARQUEIRO)
    {
        DrawRectangleRounded(
            (Rectangle){x - 12, y - 7, 24, 19},
            0.12f,
            4,
            (Color){88, 61, 37, 255});

        DrawRectangle((int)x - 8, (int)y - 14, 16, 9, (Color){124, 89, 48, 255});

        DrawLineEx((Vector2){x - 14, y - 17}, (Vector2){x + 14, y + 15}, 2, cor);
        DrawLineEx((Vector2){x + 14, y - 17}, (Vector2){x - 14, y + 15}, 2, cor);
    }
    else if (tipo == TORRE_GELO)
    {
        DrawPoly((Vector2){x, y}, 6, 16, 0, (Color){55, 145, 190, 255});
        DrawPoly((Vector2){x, y}, 6, 10, 0, (Color){155, 225, 255, 255});
        DrawCircle((int)x, (int)y, 4, WHITE);
    }
    else
    {
        DrawPoly((Vector2){x, y}, 6, 16, 0, (Color){73, 42, 112, 255});
        DrawPoly((Vector2){x, y}, 6, 10, 0, COR_ROXO);

        DrawLineEx((Vector2){x - 2, y - 12}, (Vector2){x + 5, y - 2}, 3, (Color){230, 190, 255, 255});
        DrawLineEx((Vector2){x + 5, y - 2}, (Vector2){x - 4, y + 2}, 3, (Color){230, 190, 255, 255});
        DrawLineEx((Vector2){x - 4, y + 2}, (Vector2){x + 2, y + 12}, 3, (Color){230, 190, 255, 255});
    }
}

static void desenharCartaoTorre(Rectangle area, TipoTorre tipo, const char *nome, const char *preco, Color cor, int selecionada)
{
    Color fundo = selecionada
                      ? (Color){57, 49, 34, 255}
                      : (Color){22, 34, 35, 255};

    DrawRectangleRounded(area, 0.13f, 8, fundo);

    DrawRectangleRoundedLinesEx(
        area,
        0.13f,
        8,
        selecionada ? 3 : 1.5f,
        selecionada ? GOLD : COR_BORDA);

    if (selecionada)
    {
        DrawRectangleRounded(
            (Rectangle){area.x + 4, area.y + 4, 5, area.height - 8},
            0.5f,
            4,
            cor);
    }

    desenharIconeTorre(tipo, area.x + 30, area.y + 34, cor);

    desenharTextoUI(nome, area.x + 55, area.y + 10, 19, RAYWHITE);
    desenharTextoUI(preco, area.x + 55, area.y + 37, 17, (Color){220, 190, 125, 255});

    DrawCircle(
        (int)(area.x + area.width - 17),
        (int)(area.y + 17),
        4,
        selecionada ? GOLD : (Color){90, 100, 94, 255});
}

static void desenharPainelTorre(Torre torres[], int indice)
{
    if (indice < 0 || indice >= MAX_TORRES || !torres[indice].ativo)
        return;

    Torre torre = torres[indice];
    int valorVenda = torre.valorInvestido / 2;

    const char *nome;
    Color cor;

    switch (torre.tipo)
    {
    case TORRE_ARQUEIRO:
        nome = "TORRE ARQUEIRO";
        cor = (Color){194, 151, 78, 255};
        break;

    case TORRE_GELO:
        nome = "TORRE DE GELO";
        cor = (Color){105, 194, 235, 255};
        break;

    case TORRE_TROVAO:
        nome = "TORRE DO TROVAO";
        cor = COR_ROXO;
        break;

    default:
        nome = "TORRE";
        cor = GOLD;
        break;
    }

    desenharPainel((Rectangle){510, 627, 750, 84}, (Color){13, 23, 26, 250});

    desenharTextoUI(nome, 530, 634, 19, cor);
    desenharTextoUI(TextFormat("NIVEL %d / 3", torre.nivel), 530, 662, 16, RAYWHITE);
    desenharTextoUI(TextFormat("DANO  %d", torre.dano), 700, 636, 17, RAYWHITE);
    desenharTextoUI(TextFormat("ALCANCE  %.0f", torre.alcance), 700, 663, 15, (Color){190, 205, 205, 255});
    desenharTextoUI(TextFormat("VENDA  %d", valorVenda), 865, 636, 17, GOLD);
    desenharTextoUI("50% DO INVESTIMENTO", 865, 665, 14, LIGHTGRAY);

    Rectangle botaoUpgrade = {1015, 635, 110, 65};
    Rectangle botaoVenda = {1135, 635, 115, 65};

    Color fundoUpgrade = torre.nivel >= 3
                             ? (Color){45, 48, 45, 255}
                             : (Color){47, 83, 62, 255};

    DrawRectangleRounded(botaoUpgrade, 0.16f, 8, fundoUpgrade);
    DrawRectangleRoundedLinesEx(botaoUpgrade, 0.16f, 8, 2, torre.nivel >= 3 ? DARKGRAY : (Color){155, 193, 123, 255});

    DrawRectangleRounded(botaoVenda, 0.16f, 8, (Color){85, 38, 38, 255});
    DrawRectangleRoundedLinesEx(botaoVenda, 0.16f, 8, 2, (Color){180, 91, 72, 255});

    if (torre.nivel >= 3)
    {
        desenharTextoUI("NIVEL MAX", 1020, 657, 15, LIGHTGRAY);
    }
    else
    {
        desenharTextoUI("MELHORAR", 1020, 641, 15, WHITE);
        desenharTextoUI(TextFormat("%d moedas", torre.custo * torre.nivel), 1020, 671, 14, (Color){225, 205, 150, 255});
    }

    desenharTextoUI("VENDER", 1150, 642, 17, WHITE);
    desenharTextoUI(TextFormat("+%d moedas", valorVenda), 1145, 671, 14, (Color){239, 183, 132, 255});
}

static void desenharPainelConstrucoes(TipoTorre tipoTorreSelecionada)
{
    DrawRectangleGradientV(
        0, 620,
        LARGURA_TELA, 100,
        (Color){21, 32, 32, 255},
        (Color){10, 19, 22, 255});

    DrawRectangle(0, 620, LARGURA_TELA, 2, COR_BORDA);

    desenharTextoUI("CONSTRUIR", 22, 622, 16, (Color){174, 155, 116, 255});

    desenharCartaoTorre(
        (Rectangle){20, 640, 150, 65},
        TORRE_ARQUEIRO,
        "ARQUEIRO",
        "50 moedas",
        (Color){194, 151, 78, 255},
        tipoTorreSelecionada == TORRE_ARQUEIRO);

    desenharCartaoTorre(
        (Rectangle){180, 640, 150, 65},
        TORRE_GELO,
        "GELO",
        "65 moedas",
        (Color){105, 194, 235, 255},
        tipoTorreSelecionada == TORRE_GELO);

    desenharCartaoTorre(
        (Rectangle){340, 640, 150, 65},
        TORRE_TROVAO,
        "TROVAO",
        "90 moedas",
        COR_ROXO,
        tipoTorreSelecionada == TORRE_TROVAO);
}

static void desenharPainelOnda(int ondaIniciada, int inimigosCriados, int totalInimigosOnda)
{
    if (!ondaIniciada)
    {
        desenharTextoUI(
            "PRESSIONE ESPACO PARA INICIAR A ONDA",
            650, 653, 17,
            (Color){213, 194, 148, 255});
    }
    else
    {
        desenharTextoUI("INIMIGOS", 650, 638, 16, LIGHTGRAY);

        desenharTextoTitulo(
            TextFormat("%d / %d", inimigosCriados, totalInimigosOnda),
            650, 658, 24,
            SKYBLUE);
    }
}

static void desenharTelaFinal(int vitoria, int derrota)
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){0, 0, 0, 190});

    desenharPainel(
        (Rectangle){300, 210, 680, 300},
        (Color){13, 23, 27, 250});

    if (vitoria)
    {
        desenharTextoCentralizado("VITORIA!", fonteTitulo, 75, 270, GOLD);
        desenharTextoCentralizado("Yggdrasil foi salvo!", fonteInterface, 30, 370, RAYWHITE);
        desenharTextoCentralizado("As dez ondas foram superadas.", fonteInterface, 21, 420, (Color){190, 205, 185, 255});
    }
    else if (derrota)
    {
        desenharTextoCentralizado("DERROTA", fonteTitulo, 75, 270, (Color){210, 75, 70, 255});
        desenharTextoCentralizado("Yggdrasil foi destruido!", fonteInterface, 30, 370, RAYWHITE);
        desenharTextoCentralizado("Ragnarok consumiu os nove mundos.", fonteInterface, 21, 420, (Color){190, 175, 175, 255});
    }

    desenharTextoCentralizado("Escolha uma opcao", fonteInterface, 20, 455, GOLD);
}

static int desenharMenuPausa(const Jogo *jogo)
{
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){0, 0, 0, 205});
    desenharPainel((Rectangle){350, 65, 580, 590}, (Color){13, 23, 27, 252});
    desenharTextoCentralizado("JOGO PAUSADO", fonteTitulo, 43, 88, GOLD);
    desenharTextoCentralizado("Sua jornada esta segura enquanto voce decide.", fonteInterface, 20, 143, COR_TEXTO);
    desenharTextoCentralizado(TextFormat("Estado atual: onda %d  |  %d moedas  |  %d de vida", jogo->onda, jogo->moedas, jogo->vidaBase), fonteInterface, 17, 177, (Color){190, 210, 195, 255});

    if (desenharBotao((Rectangle){490, 220, 300, 52}, "CONTINUAR", 1))
        return 1;
    if (desenharBotao((Rectangle){490, 282, 300, 52}, "SALVAR PARTIDA", 0))
        return 2;
    if (desenharBotao((Rectangle){490, 344, 300, 52}, "CARREGAR PARTIDA", 0))
        return 3;
    if (desenharBotao((Rectangle){490, 406, 300, 52}, "REINICIAR DO INICIO", 0))
        return 4;
    if (desenharBotao((Rectangle){490, 468, 300, 52}, "VOLTAR AO MENU", 0))
        return 5;
    desenharTextoCentralizado(mensagemPausa, fonteInterface, 16, 550, (Color){232, 211, 155, 255});
    return 0;
}

static int desenharBotoesTelaFinal(void)
{
    if (desenharBotao((Rectangle){350, 515, 270, 66}, "JOGAR NOVAMENTE", 1))
        return 1;
    if (desenharBotao((Rectangle){660, 515, 270, 66}, "MENU INICIAL", 0))
        return 2;
    return 0;
}

static void desenharDestaqueTorreSelecionada(const Torre torres[], int indice)
{
    if (indice < 0 || indice >= MAX_TORRES || !torres[indice].ativo)
        return;

    float x = (float)(ORIGEM_X + torres[indice].coluna * LARGURA_CELULA);
    float y = (float)(ORIGEM_Y + torres[indice].linha * ALTURA_CELULA);
    float pulso = 0.5f + 0.5f * sinf((float)GetTime() * 5.0f);
    unsigned char alpha = (unsigned char)(150.0f + pulso * 105.0f);
    Color brilho = (Color){255, 218, 105, alpha};
    Rectangle celula = {x + 3, y + 3, LARGURA_CELULA - 6, ALTURA_CELULA - 6};

    DrawRectangleLinesEx(celula, 4.0f, brilho);
    DrawRectangleLinesEx((Rectangle){x + 8, y + 8, LARGURA_CELULA - 16, ALTURA_CELULA - 16}, 1.5f, (Color){255, 245, 190, 210});
    DrawCircleLines((int)(x + LARGURA_CELULA / 2), (int)(y + ALTURA_CELULA / 2), 25.0f + pulso * 4.0f, brilho);
}

static void desenharPartida(
    Jogo *jogo,
    Inimigo inimigos[],
    Torre torres[],
    int ondaIniciada,
    int inimigosCriados,
    int totalInimigosOnda,
    int vitoria,
    int derrota,
    TipoTorre tipoTorreSelecionada,
    int torreSelecionada)
{
    desenharCenarioCompleto();
    desenharHUD(jogo);

    desenharMapa(jogo, ORIGEM_X, ORIGEM_Y, LARGURA_CELULA, ALTURA_CELULA);

    desenharTorres(torres, MAX_TORRES, ORIGEM_X, ORIGEM_Y, LARGURA_CELULA, ALTURA_CELULA);
    desenharDestaqueTorreSelecionada(torres, torreSelecionada);

    desenharInimigos(inimigos, MAX_INIMIGOS, jogo, ORIGEM_X, ORIGEM_Y, LARGURA_CELULA, ALTURA_CELULA);

    desenharPainelConstrucoes(tipoTorreSelecionada);
    desenharPainelTorre(torres, torreSelecionada);

    if (torreSelecionada == -1)
    {
        desenharPainelOnda(ondaIniciada, inimigosCriados, totalInimigosOnda);
    }

    if (vitoria || derrota)
        desenharTelaFinal(vitoria, derrota);

    DrawFPS(1190, 690);
}

int main(void)
{
    Jogo jogo;
    Inimigo *inimigos = malloc(MAX_INIMIGOS * sizeof(Inimigo));
    Torre *torres = malloc(MAX_TORRES * sizeof(Torre));

    TipoTorre tipoTorreSelecionada = TORRE_ARQUEIRO;
    int torreSelecionada = -1;

    EstadoTela telaAtual = MENU_INICIAL;

    Onda onda;

    int vitoria = 0;
    int derrota = 0;
    int resultadoRegistrado = 0;
    char nomeJogador[TAM_NOME_JOGADOR] = "";
    int campoNomeSelecionado = 1;

    if (inimigos == NULL || torres == NULL)
    {
        free(inimigos);
        free(torres);
        return 1;
    }

    inicializarJogo(&jogo);
    inicializarOnda(&onda);

    srand((unsigned int)time(NULL));

    inicializarMapa(&jogo);
    inicializarInimigos(inimigos, MAX_INIMIGOS);
    inicializarTorres(torres, MAX_TORRES);

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Ragnarok: Ultima Defesa");

    SetTargetFPS(FPS);
    SetExitKey(KEY_NULL);

    fonteTitulo = LoadFontEx("C:/Windows/Fonts/timesbd.ttf", 96, NULL, 0);
    if (fonteTitulo.texture.id == 0)
        fonteTitulo = LoadFontEx("assets/fontes/TimesNewRoman-Bold.ttf", 96, NULL, 0);

    fonteInterface = LoadFontEx("C:/Windows/Fonts/times.ttf", 48, NULL, 0);
    if (fonteInterface.texture.id == 0)
        fonteInterface = LoadFontEx("assets/fontes/TimesNewRoman.ttf", 48, NULL, 0);

    if (fonteTitulo.texture.id != 0)
    {
        fonteTituloCarregada = 1;
        SetTextureFilter(fonteTitulo.texture, TEXTURE_FILTER_BILINEAR);
    }
    else
    {
        fonteTitulo = GetFontDefault();
    }

    fonteInterfaceCarregada = fonteInterface.texture.id != 0;
    if (!fonteInterfaceCarregada)
        fonteInterface = GetFontDefault();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (telaAtual == MENU_INICIAL)
        {
            BeginDrawing();

            int escolha = desenharMenuInicial();

            EndDrawing();

            if (escolha == 1)
            {
                inicializarJogo(&jogo);
                inicializarOnda(&onda);
                inicializarMapa(&jogo);
                inicializarInimigos(inimigos, MAX_INIMIGOS);
                inicializarTorres(torres, MAX_TORRES);
                tipoTorreSelecionada = TORRE_ARQUEIRO;
                torreSelecionada = -1;
                vitoria = 0;
                derrota = 0;
                resultadoRegistrado = 0;
                nomeJogador[0] = '\0';
                campoNomeSelecionado = 1;
                snprintf(mensagemPausa, sizeof(mensagemPausa), "A partida esta pausada.");
                telaAtual = EM_JOGO;
            }
            else if (escolha == 2)
            {
                telaAtual = MENU_INSTRUCOES;
            }
            else if (escolha == 3)
            {
                break;
            }
            else if (escolha == 4)
            {
                telaAtual = MENU_RECORDE;
            }
        }
        else if (telaAtual == MENU_RECORDE)
        {
            BeginDrawing();
            int voltar = desenharTelaRecordes();
            EndDrawing();
            if (voltar || IsKeyPressed(KEY_ESCAPE))
                telaAtual = MENU_INICIAL;
        }
        else if (telaAtual == MENU_INSTRUCOES)
        {
            BeginDrawing();

            int voltar = desenharInstrucoes();

            EndDrawing();

            if (voltar || IsKeyPressed(KEY_ESCAPE))
                telaAtual = MENU_INICIAL;
        }
        else if (telaAtual == EM_JOGO)
        {
            if (!vitoria && !derrota && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(GetMousePosition(), (Rectangle){1080, 20, 175, 65}))
                telaAtual = JOGO_PAUSADO;

            if (!vitoria && !derrota && IsKeyPressed(KEY_SPACE) && !onda.iniciada)
                iniciarOnda(&onda, jogo.onda);

            if (vitoria || derrota)
            {
                if (!resultadoRegistrado)
                {
                    BeginDrawing();
                    desenharPartida(&jogo, inimigos, torres, onda.iniciada,
                                    onda.inimigosCriados, onda.totalInimigos,
                                    vitoria, derrota, tipoTorreSelecionada, torreSelecionada);
                    int confirmouNome = desenharTelaNomeJogador(nomeJogador, TAM_NOME_JOGADOR, &campoNomeSelecionado);
                    EndDrawing();

                    if (confirmouNome)
                    {
                        int ondaAlcancada = vitoria ? 10 : jogo.onda;
                        int pontuacao = ondaAlcancada * 1000 + jogo.vidaBase * 20 + jogo.moedas;
                        if (vitoria)
                            pontuacao += 5000;
                        resultadoRegistrado = registrarPartida(nomeJogador, pontuacao,
                                                               ondaAlcancada, vitoria,
                                                               jogo.vidaBase, jogo.moedas);
                    }
                    continue;
                }

                BeginDrawing();
                desenharPartida(&jogo, inimigos, torres, onda.iniciada,
                                onda.inimigosCriados, onda.totalInimigos,
                                vitoria, derrota, tipoTorreSelecionada, torreSelecionada);
                int escolhaFinal = desenharBotoesTelaFinal();
                EndDrawing();

                if (escolhaFinal == 1)
                {
                    inicializarJogo(&jogo);
                    inicializarOnda(&onda);
                    inicializarMapa(&jogo);
                    inicializarInimigos(inimigos, MAX_INIMIGOS);
                    inicializarTorres(torres, MAX_TORRES);
                    torreSelecionada = -1;
                    tipoTorreSelecionada = TORRE_ARQUEIRO;
                    vitoria = 0;
                    derrota = 0;
                    resultadoRegistrado = 0;
                    nomeJogador[0] = '\0';
                    campoNomeSelecionado = 1;
                }
                else if (escolhaFinal == 2)
                    telaAtual = MENU_INICIAL;
                continue;
            }

            if (!vitoria && !derrota &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();
                Rectangle botaoMelhoria = {1015, 635, 110, 65};
                Rectangle botaoVenda = {1135, 635, 115, 65};

                if (torreSelecionada != -1 &&
                    CheckCollisionPointRec(mouse, botaoVenda))
                {
                    if (venderTorre(torres, MAX_TORRES, &jogo, torreSelecionada))
                        torreSelecionada = -1;
                }
                else if (torreSelecionada != -1 &&
                         CheckCollisionPointRec(mouse, botaoMelhoria))
                {
                    melhorarTorre(torres, MAX_TORRES, &jogo, torreSelecionada);
                }
                else if (mouse.y >= 620 && mouse.y <= 720)
                {
                    if (mouse.x >= 20 && mouse.x <= 170)
                    {
                        tipoTorreSelecionada = (tipoTorreSelecionada == TORRE_ARQUEIRO) ? -1 : TORRE_ARQUEIRO;
                        torreSelecionada = -1;
                    }
                    else if (mouse.x >= 180 && mouse.x <= 330)
                    {
                        tipoTorreSelecionada = (tipoTorreSelecionada == TORRE_GELO) ? -1 : TORRE_GELO;
                        torreSelecionada = -1;
                    }
                    else if (mouse.x >= 340 && mouse.x <= 490)
                    {
                        tipoTorreSelecionada = (tipoTorreSelecionada == TORRE_TROVAO) ? -1 : TORRE_TROVAO;
                        torreSelecionada = -1;
                    }
                }
                else if (mouse.x >= ORIGEM_X &&
                         mouse.x < ORIGEM_X + COLUNAS * LARGURA_CELULA &&
                         mouse.y >= ORIGEM_Y &&
                         mouse.y < ORIGEM_Y + LINHAS * ALTURA_CELULA)
                {
                    int coluna = (int)((mouse.x - ORIGEM_X) / LARGURA_CELULA);
                    int linha = (int)((mouse.y - ORIGEM_Y) / ALTURA_CELULA);
                    int indice = selecionarTorre(torres, MAX_TORRES, linha, coluna);

                    if (indice != -1)
                    {
                        torreSelecionada = indice;
                    }
                    else
                    {
                        torreSelecionada = -1;

                        if (tipoTorreSelecionada != -1)
                        {
                            construirTorre(torres, MAX_TORRES, &jogo, linha, coluna,
                                           tipoTorreSelecionada);
                        }
                    }
                }
            }

            if (!derrota && !vitoria)
            {
                atualizarOnda(&onda, jogo.onda, inimigos, MAX_INIMIGOS, deltaTime);
            }

            if (!derrota && !vitoria)
            {
                atualizarInimigos(inimigos, MAX_INIMIGOS, &jogo, deltaTime);
            }

            if (!derrota && !vitoria)
            {
                atualizarTorres(
                    torres,
                    MAX_TORRES,
                    inimigos,
                    MAX_INIMIGOS,
                    &jogo,
                    deltaTime,
                    ORIGEM_X,
                    ORIGEM_Y,
                    LARGURA_CELULA,
                    ALTURA_CELULA);
            }

            if (!derrota && !vitoria && jogo.vidaBase <= 0)
            {
                jogo.vidaBase = 0;
                derrota = 1;
            }
            else if (!derrota && !vitoria &&
                     verificarFimOnda(&onda, inimigos, MAX_INIMIGOS))
            {
                if (jogo.onda == 10)
                    vitoria = 1;
                else
                    jogo.onda++;
            }

            BeginDrawing();

            desenharPartida(
                &jogo,
                inimigos,
                torres,
                onda.iniciada,
                onda.inimigosCriados,
                onda.totalInimigos,
                vitoria,
                derrota,
                tipoTorreSelecionada,
                torreSelecionada);

            if (!onda.iniciada && !vitoria && !derrota)
                desenharMensagem("Pressione ESPACO para iniciar a proxima onda");
            EndDrawing();
        }
        else if (telaAtual == JOGO_PAUSADO)
        {
            BeginDrawing();
            desenharPartida(&jogo, inimigos, torres, onda.iniciada,
                            onda.inimigosCriados, onda.totalInimigos,
                            vitoria, derrota, tipoTorreSelecionada, torreSelecionada);
            int acaoPausa = desenharMenuPausa(&jogo);
            EndDrawing();

            if (acaoPausa == 1)
                telaAtual = EM_JOGO;
            else if (acaoPausa == 2)
            {
                if (salvarPartida("salvamento_partida.dat", &jogo, &onda, inimigos, torres, vitoria, derrota))
                    snprintf(mensagemPausa, sizeof(mensagemPausa), "Partida salva! O arquivo salvamento_partida.dat foi atualizado.");
                else
                    snprintf(mensagemPausa, sizeof(mensagemPausa), "Nao foi possivel salvar. Verifique a pasta do jogo.");
            }
            else if (acaoPausa == 3)
            {
                if (carregarPartida("salvamento_partida.dat", &jogo, &onda, inimigos, torres, &vitoria, &derrota))
                {
                    inicializarMapa(&jogo);
                    torreSelecionada = -1;
                    snprintf(mensagemPausa, sizeof(mensagemPausa), "Partida carregada! Voce voltou ao ponto salvo.");
                    telaAtual = EM_JOGO;
                }
                else
                    snprintf(mensagemPausa, sizeof(mensagemPausa), "Nao existe salvamento valido para carregar.");
            }
            else if (acaoPausa == 4)
            {
                inicializarJogo(&jogo);
                inicializarOnda(&onda);
                inicializarMapa(&jogo);
                inicializarInimigos(inimigos, MAX_INIMIGOS);
                inicializarTorres(torres, MAX_TORRES);
                tipoTorreSelecionada = TORRE_ARQUEIRO;
                torreSelecionada = -1;
                vitoria = 0;
                derrota = 0;
                resultadoRegistrado = 0;
                nomeJogador[0] = '\0';
                campoNomeSelecionado = 1;
                snprintf(mensagemPausa, sizeof(mensagemPausa), "Nova partida iniciada.");
                telaAtual = EM_JOGO;
            }
            else if (acaoPausa == 5)
                telaAtual = MENU_INICIAL;
        }
    }

    if (fonteTituloCarregada)
        UnloadFont(fonteTitulo);

    if (fonteInterfaceCarregada)
        UnloadFont(fonteInterface);

    CloseWindow();

    free(inimigos);
    free(torres);

    return 0;
}

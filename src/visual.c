
#include <math.h>
#include "raylib.h"
#include "visual.h"

/* Cores personalizadas */
static const Color CEU = {12, 19, 35, 255};
static const Color CEU_CLARO = {28, 39, 57, 255};
static const Color MONTANHA = {22, 31, 43, 255};
static const Color MONTANHA_CLARA = {34, 43, 53, 255};
static const Color TRONCO = {75, 52, 42, 255};
static const Color TRONCO_CLARO = {112, 77, 53, 255};
static const Color FOLHA = {27, 73, 60, 255};
static const Color FOLHA_CLARA = {43, 111, 77, 255};
static const Color MAGIA = {94, 231, 190, 255};
static const Color GRAMA = {27, 43, 36, 255};

/* Desenha uma linha grossa com aparência de galho */
static void desenharGalho(Vector2 inicio, Vector2 fim,
                          float grossura, Color cor)
{
    DrawLineEx(inicio, fim, grossura, cor);
}

/* Desenha uma montanha com vários polígonos */
static void desenharMontanhas(void)
{
    Vector2 montanha1[] = {
        {0, 390}, {150, 190}, {280, 350}, {430, 160}, {620, 390}, {0, 390}};

    Vector2 montanha2[] = {
        {400, 390}, {610, 210}, {770, 350}, {960, 170}, {1160, 390}, {400, 390}};

    Vector2 montanha3[] = {
        {850, 390}, {1050, 220}, {1200, 340}, {1280, 250}, {1280, 390}, {850, 390}};

    DrawTriangleFan(montanha1, 6, MONTANHA);
    DrawTriangleFan(montanha2, 6, MONTANHA_CLARA);
    DrawTriangleFan(montanha3, 6, MONTANHA);
}

/* Lua */
static void desenharLua(void)
{
    DrawCircleGradient(
        (Vector2){1030, 115},
        78,
        (Color){130, 190, 175, 35},
        BLANK);

    DrawCircle(1030, 115, 43, (Color){205, 218, 190, 255});
    DrawCircle(1047, 102, 40, CEU);

    DrawCircle(1005, 120, 4, (Color){160, 177, 155, 100});
    DrawCircle(1020, 92, 3, (Color){160, 177, 155, 100});
}

/* Estrelas */
static void desenharEstrelas(void)
{
    for (int i = 0; i < 65; i++)
    {
        int x = (i * 197 + 53) % 1280;
        int y = (i * 83 + 31) % 260;

        float brilho = 0.45f + 0.45f * sinf((float)i * 2.1f);

        Color cor = {
            180, 210, 205,
            (unsigned char)(brilho * 210)};

        DrawCircle((float)x, (float)y, 1.5f, cor);
    }
}

/* Árvores distantes */
static void desenharArvoreDistante(float x, float y, float escala)
{
    DrawRectangle(
        (int)(x - 4 * escala),
        (int)(y - 5 * escala),
        (int)(8 * escala),
        (int)(45 * escala),
        (Color){40, 43, 39, 255});

    DrawCircle((int)x, (int)(y - 35 * escala),
               23 * escala, (Color){19, 48, 42, 255});

    DrawCircle((int)(x - 13 * escala),
               (int)(y - 22 * escala),
               17 * escala, (Color){23, 57, 46, 255});

    DrawCircle((int)(x + 14 * escala),
               (int)(y - 20 * escala),
               17 * escala, (Color){17, 42, 38, 255});
}

/* Floresta ao fundo */
static void desenharFloresta(void)
{
    for (int i = 0; i < 15; i++)
    {
        float x = (float)(i * 95 - 20);
        float y = 405 + (float)((i % 3) * 12);
        float escala = 0.75f + (float)(i % 3) * 0.15f;

        desenharArvoreDistante(x, y, escala);
    }
}

/* Desenha o céu e o ambiente distante */
void desenharCenarioFundo(void)
{
    DrawRectangleGradientV(
        0, 0, 1280, 720,
        CEU_CLARO, CEU);

    desenharEstrelas();
    desenharLua();
    desenharMontanhas();
    desenharFloresta();

    /* Névoa na linha do horizonte */
    DrawRectangleGradientV(
        0, 340, 1280, 100,
        (Color){65, 104, 91, 0},
        (Color){65, 104, 91, 45});
}

/* Tronco principal da Yggdrasil */
static void desenharTronco(void)
{
    Vector2 pontos[] = {
        {1080, 510},
        {1100, 400},
        {1080, 300},
        {1120, 220},
        {1160, 300},
        {1150, 410},
        {1190, 510}};

    DrawTriangleFan(pontos, 7, TRONCO);

    DrawLineEx((Vector2){1110, 490},
               (Vector2){1120, 290}, 17, TRONCO_CLARO);

    DrawLineEx((Vector2){1140, 470},
               (Vector2){1140, 330}, 9, TRONCO);

    /* Textura do tronco */
    DrawLineEx((Vector2){1098, 380},
               (Vector2){1120, 350}, 4, (Color){135, 94, 58, 255});

    DrawLineEx((Vector2){1125, 435},
               (Vector2){1145, 405}, 4, (Color){135, 94, 58, 255});
}

/* Raízes */
static void desenharRaizes(void)
{
    desenharGalho((Vector2){1120, 480},
                  (Vector2){1030, 570}, 25, TRONCO);

    desenharGalho((Vector2){1120, 480},
                  (Vector2){1090, 600}, 22, TRONCO);

    desenharGalho((Vector2){1140, 480},
                  (Vector2){1210, 575}, 25, TRONCO);

    desenharGalho((Vector2){1135, 480},
                  (Vector2){1170, 620}, 17, TRONCO_CLARO);

    desenharGalho((Vector2){1080, 530},
                  (Vector2){990, 600}, 12, TRONCO);
}

/* Galhos superiores */
static void desenharGalhos(void)
{
    desenharGalho((Vector2){1120, 290},
                  (Vector2){1000, 210}, 19, TRONCO);

    desenharGalho((Vector2){1000, 210},
                  (Vector2){950, 140}, 12, TRONCO_CLARO);

    desenharGalho((Vector2){1000, 210},
                  (Vector2){900, 190}, 10, TRONCO);

    desenharGalho((Vector2){1130, 280},
                  (Vector2){1230, 180}, 17, TRONCO);

    desenharGalho((Vector2){1230, 180},
                  (Vector2){1260, 110}, 10, TRONCO_CLARO);

    desenharGalho((Vector2){1150, 320},
                  (Vector2){1190, 220}, 13, TRONCO);
}

/* Folhagem */
static void desenharFolhagem(void)
{
    Vector2 grupos[] = {
        {900, 170}, {960, 125}, {1010, 185}, {1050, 130}, {1100, 110}, {1150, 145}, {1200, 110}, {1250, 145}, {1190, 210}};

    for (int i = 0; i < 9; i++)
    {
        float x = grupos[i].x;
        float y = grupos[i].y;

        DrawCircleGradient(
            (Vector2){x, y},
            52,
            FOLHA_CLARA,
            FOLHA);

        DrawCircle((int)(x - 18), (int)(y - 12),
                   25, FOLHA_CLARA);

        DrawCircle((int)(x + 18), (int)(y + 12),
                   27, FOLHA);
    }
}

/* Energia mágica da árvore */
static void desenharEnergiaYggdrasil(void)
{
    float tempo = (float)GetTime();

    for (int i = 0; i < 8; i++)
    {
        float deslocamento = sinf(tempo * 1.5f + (float)i) * 8.0f;

        float x = 1080 + (float)(i * 22);
        float y = 320 + deslocamento + (float)((i % 3) * 35);

        DrawCircleGradient(
            (Vector2){x, y},
            14,
            (Color){94, 231, 190, 90},
            BLANK);

        DrawCircle((int)x, (int)y, 3, MAGIA);
    }
}

/* Desenha a Yggdrasil completa */
void desenharYggdrasil(void)
{
    /* Aura */
    DrawCircleGradient(
        (Vector2){1130, 330},
        210,
        (Color){35, 150, 115, 45},
        BLANK);

    desenharRaizes();
    desenharTronco();
    desenharGalhos();
    desenharFolhagem();
    desenharEnergiaYggdrasil();
}

/* Terreno */
static void desenharTerreno(void)
{
    DrawRectangleGradientV(
        0, 510, 1280, 210,
        (Color){39, 57, 43, 255},
        (Color){13, 27, 25, 255});

    DrawRectangle(
        0, 510, 1280, 5,
        (Color){69, 95, 62, 255});

    /* Pequenas pedras */
    for (int i = 0; i < 25; i++)
    {
        int x = (i * 113 + 35) % 1280;
        int y = 555 + (i * 37) % 140;

        DrawEllipse(x, y, 12, 6,
                    (Color){48, 57, 49, 255});
    }

    /* Pontos de vegetação */
    for (int i = 0; i < 70; i++)
    {
        int x = (i * 79 + 15) % 1280;
        int y = 525 + (i * 43) % 180;

        DrawLine(x, y, x - 3, y - 9, GRAMA);
        DrawLine(x, y, x + 4, y - 11, FOLHA_CLARA);
    }
}

/* Cenário completo */
void desenharCenarioCompleto(void)
{
    desenharCenarioFundo();
    desenharYggdrasil();
    desenharTerreno();
}

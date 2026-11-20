
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"
#include "jogo.h"
#include "mapa.h"
#include "inimigo.h"
#include "visual.h"
#include "onda.h"
#include "torre.h"

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
    EM_JOGO
} EstadoTela;

static Font fonteTitulo;
static Font fonteInterface;

static int fonteTituloCarregada = 0;
static int fonteInterfaceCarregada = 0;

static const Color COR_FUNDO = {12, 25, 29, 255};
static const Color COR_DOURADO = {190, 151, 78, 255};
static const Color COR_PELE = {204, 166, 132, 255};

static void desenharTextoCentralizado(
    const char *texto,
    Font fonte,
    float tamanho,
    float y,
    Color cor)
{
    Vector2 dimensoes = MeasureTextEx(
        fonte,
        texto,
        tamanho,
        1);

    DrawTextEx(
        fonte,
        texto,
        (Vector2){
            (LARGURA_TELA - dimensoes.x) / 2.0f,
            y},
        tamanho,
        1,
        cor);
}

static int desenharBotao(
    Rectangle botao,
    const char *texto,
    int destaque)
{
    Vector2 mouse = GetMousePosition();

    int sobre = CheckCollisionPointRec(mouse, botao);

    Color fundo = (Color){15, 27, 30, 240};
    Color borda = COR_DOURADO;
    Color corTexto = RAYWHITE;

    if (destaque)
        fundo = (Color){54, 47, 29, 245};

    if (sobre)
    {
        fundo = (Color){83, 68, 37, 250};
        borda = GOLD;
        corTexto = GOLD;
    }

    DrawRectangleRounded(
        botao,
        0.18f,
        8,
        fundo);

    DrawRectangleLinesEx(
        botao,
        sobre ? 3 : 2,
        borda);

    Vector2 tamanhoTexto = MeasureTextEx(
        fonteInterface,
        texto,
        22,
        1);

    DrawTextEx(
        fonteInterface,
        texto,
        (Vector2){
            botao.x + (botao.width - tamanhoTexto.x) / 2,
            botao.y + (botao.height - tamanhoTexto.y) / 2},
        22,
        1,
        corTexto);

    return sobre &&
           IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

static void desenharMontanhas(void)
{
    DrawTriangle(
        (Vector2){0, 330},
        (Vector2){220, 95},
        (Vector2){440, 330},
        (Color){35, 58, 65, 255});

    DrawTriangle(
        (Vector2){240, 330},
        (Vector2){520, 125},
        (Vector2){800, 330},
        (Color){43, 67, 70, 255});

    DrawTriangle(
        (Vector2){650, 330},
        (Vector2){940, 85},
        (Vector2){1220, 330},
        (Color){34, 56, 64, 255});

    DrawTriangle(
        (Vector2){990, 330},
        (Vector2){1160, 150},
        (Vector2){1380, 330},
        (Color){28, 48, 57, 255});

    DrawTriangle(
        (Vector2){220, 95},
        (Vector2){165, 165},
        (Vector2){270, 150},
        (Color){170, 185, 179, 255});

    DrawTriangle(
        (Vector2){520, 125},
        (Vector2){455, 190},
        (Vector2){570, 175},
        (Color){180, 192, 185, 255});

    DrawTriangle(
        (Vector2){940, 85},
        (Vector2){875, 155},
        (Vector2){1000, 160},
        (Color){180, 192, 185, 255});
}

static void desenharArvore(
    int x,
    int y,
    float escala)
{
    DrawRectangle(
        x - (int)(8 * escala),
        y,
        (int)(16 * escala),
        (int)(85 * escala),
        (Color){67, 48, 33, 255});

    DrawTriangle(
        (Vector2){x, y - 100 * escala},
        (Vector2){x - 45 * escala, y - 15 * escala},
        (Vector2){x + 45 * escala, y - 15 * escala},
        (Color){19, 66, 49, 255});

    DrawTriangle(
        (Vector2){x, y - 65 * escala},
        (Vector2){x - 55 * escala, y + 15 * escala},
        (Vector2){x + 55 * escala, y + 15 * escala},
        (Color){25, 82, 55, 255});
}

static void desenharYggdrasil(void)
{
    DrawRectangle(
        600, 135,
        80, 270,
        (Color){83, 65, 42, 255});

    DrawRectangle(
        616, 135,
        18, 270,
        (Color){111, 86, 51, 255});

    DrawLineEx(
        (Vector2){640, 205},
        (Vector2){470, 80},
        20,
        (Color){91, 73, 47, 255});

    DrawLineEx(
        (Vector2){640, 205},
        (Vector2){800, 70},
        20,
        (Color){91, 73, 47, 255});

    DrawLineEx(
        (Vector2){640, 265},
        (Vector2){430, 190},
        16,
        (Color){91, 73, 47, 255});

    DrawLineEx(
        (Vector2){640, 265},
        (Vector2){850, 180},
        16,
        (Color){91, 73, 47, 255});

    DrawCircle(640, 115, 105, (Color){20, 75, 49, 255});
    DrawCircle(555, 145, 70, (Color){27, 91, 55, 255});
    DrawCircle(720, 145, 75, (Color){24, 84, 53, 255});
    DrawCircle(640, 55, 65, (Color){31, 102, 61, 255});

    DrawCircle(640, 135, 7, GOLD);
    DrawCircle(570, 110, 4, (Color){130, 190, 155, 255});
    DrawCircle(710, 95, 4, (Color){130, 190, 155, 255});
}

static void desenharMonstroDraugr(int x, int y)
{
    Color pele = (Color){80, 112, 82, 255};

    DrawCircle(x, y, 30, pele);

    DrawRectangle(
        x - 27, y + 20,
        54, 55,
        (Color){40, 63, 49, 255});

    DrawCircle(x - 10, y, 5, RED);
    DrawCircle(x + 10, y, 5, RED);

    DrawLine(
        x - 10, y + 15,
        x + 10, y + 15,
        BLACK);
}

static void desenharMonstroFenrir(int x, int y)
{
    Color pelo = (Color){73, 83, 82, 255};

    DrawCircle(x, y, 35, pelo);

    DrawTriangle(
        (Vector2){x - 28, y - 15},
        (Vector2){x - 42, y - 60},
        (Vector2){x - 5, y - 32},
        pelo);

    DrawTriangle(
        (Vector2){x + 28, y - 15},
        (Vector2){x + 42, y - 60},
        (Vector2){x + 5, y - 32},
        pelo);

    DrawCircle(x - 12, y, 5, GOLD);
    DrawCircle(x + 12, y, 5, GOLD);

    DrawTriangle(
        (Vector2){x, y + 15},
        (Vector2){x - 12, y + 5},
        (Vector2){x + 12, y + 5},
        (Color){35, 38, 38, 255});
}

static void desenharMonstroTroll(int x, int y)
{
    Color pele = (Color){88, 113, 75, 255};

    DrawCircle(x, y - 25, 35, pele);

    DrawRectangle(
        x - 42, y,
        84, 85,
        (Color){67, 88, 59, 255});

    DrawCircle(x - 12, y - 30, 5, GOLD);
    DrawCircle(x + 12, y - 30, 5, GOLD);

    DrawTriangle(
        (Vector2){x - 20, y - 50},
        (Vector2){x - 38, y - 78},
        (Vector2){x - 5, y - 60},
        (Color){190, 185, 150, 255});

    DrawTriangle(
        (Vector2){x + 20, y - 50},
        (Vector2){x + 38, y - 78},
        (Vector2){x + 5, y - 60},
        (Color){190, 185, 150, 255});
}

static void desenharMonstroHel(int x, int y)
{
    Color armadura = (Color){56, 67, 91, 255};

    DrawCircle(x, y, 29, armadura);

    DrawRectangle(
        x - 32, y + 20,
        64, 65,
        armadura);

    DrawTriangle(
        (Vector2){x - 20, y - 18},
        (Vector2){x - 42, y - 65},
        (Vector2){x - 5, y - 38},
        armadura);

    DrawTriangle(
        (Vector2){x + 20, y - 18},
        (Vector2){x + 42, y - 65},
        (Vector2){x + 5, y - 38},
        armadura);

    DrawCircle(x - 10, y, 5, SKYBLUE);
    DrawCircle(x + 10, y, 5, SKYBLUE);
}

static void desenharHeroi(
    int x,
    int y,
    Color armadura,
    Color detalhe,
    int tipoArma)
{
    DrawRectangle(
        x - 19, y + 65,
        15, 42,
        (Color){35, 39, 40, 255});

    DrawRectangle(
        x + 4, y + 65,
        15, 42,
        (Color){35, 39, 40, 255});

    DrawRectangle(
        x - 24, y + 99,
        24, 12,
        (Color){30, 25, 22, 255});

    DrawRectangle(
        x + 2, y + 99,
        24, 12,
        (Color){30, 25, 22, 255});

    DrawRectangleRounded(
        (Rectangle){
            x - 30, y + 5,
            60, 72},
        0.2f,
        5,
        armadura);

    DrawRectangle(
        x - 9, y + 9,
        18, 65,
        detalhe);

    DrawCircle(
        x - 29, y + 17,
        13,
        detalhe);

    DrawCircle(
        x + 29, y + 17,
        13,
        detalhe);

    DrawRectangle(
        x - 9, y - 8,
        18, 20,
        COR_PELE);

    DrawCircle(
        x, y - 25,
        24,
        COR_PELE);

    DrawCircle(
        x, y - 38,
        23,
        (Color){48, 35, 27, 255});

    DrawRectangle(
        x - 22, y - 29,
        44, 15,
        COR_PELE);

    DrawCircle(
        x - 8, y - 26,
        2,
        BLACK);

    DrawCircle(
        x + 8, y - 26,
        2,
        BLACK);

    if (tipoArma == 0)
    {
        DrawLineEx(
            (Vector2){x + 36, y + 12},
            (Vector2){x + 65, y - 48},
            6,
            LIGHTGRAY);

        DrawLineEx(
            (Vector2){x + 27, y + 18},
            (Vector2){x + 45, y + 25},
            7,
            BROWN);
    }
    else if (tipoArma == 1)
    {
        DrawLineEx(
            (Vector2){x - 42, y + 15},
            (Vector2){x - 68, y - 55},
            7,
            BROWN);

        DrawTriangle(
            (Vector2){x - 68, y - 55},
            (Vector2){x - 92, y - 65},
            (Vector2){x - 76, y - 30},
            LIGHTGRAY);
    }
    else if (tipoArma == 2)
    {
        DrawLineEx(
            (Vector2){x + 38, y + 75},
            (Vector2){x + 38, y - 65},
            7,
            BROWN);

        DrawCircle(
            x + 38, y - 69,
            12,
            SKYBLUE);

        DrawCircle(
            x + 38, y - 69,
            5,
            WHITE);
    }
    else
    {
        DrawLineEx(
            (Vector2){x - 40, y - 45},
            (Vector2){x - 58, y + 40},
            5,
            BROWN);

        DrawLineEx(
            (Vector2){x - 58, y + 40},
            (Vector2){x - 40, y + 105},
            5,
            BROWN);

        DrawLineEx(
            (Vector2){x - 40, y - 45},
            (Vector2){x - 40, y + 105},
            2,
            LIGHTGRAY);
    }
}

static void desenharCenarioMenu(void)
{
    float tempo = (float)GetTime();

    ClearBackground(COR_FUNDO);

    DrawRectangleGradientV(
        0, 0,
        LARGURA_TELA, ALTURA_TELA,
        (Color){17, 37, 45, 255},
        (Color){9, 20, 24, 255});

    DrawCircle(
        1040, 115,
        48,
        (Color){180, 190, 172, 255});

    DrawCircle(
        1058, 100,
        45,
        (Color){17, 37, 45, 255});

    desenharMontanhas();
    desenharYggdrasil();

    DrawRectangle(
        0, 390,
        LARGURA_TELA, 330,
        (Color){20, 48, 37, 255});

    desenharArvore(70, 430, 1.2f);
    desenharArvore(190, 450, 0.9f);
    desenharArvore(1090, 435, 1.2f);
    desenharArvore(1200, 450, 0.9f);

    desenharMonstroDraugr(315, 360);
    desenharMonstroFenrir(470, 365);
    desenharMonstroTroll(800, 350);
    desenharMonstroHel(960, 355);

    desenharHeroi(
        410, 405,
        (Color){83, 99, 107, 255},
        (Color){164, 130, 78, 255},
        0);

    desenharHeroi(
        545, 405,
        (Color){86, 93, 64, 255},
        (Color){170, 150, 93, 255},
        1);

    desenharHeroi(
        680, 405,
        (Color){54, 75, 107, 255},
        (Color){110, 169, 190, 255},
        2);

    desenharHeroi(
        815, 405,
        (Color){92, 62, 57, 255},
        (Color){183, 135, 77, 255},
        3);

    for (int i = 0; i < 18; i++)
    {
        float x = (float)((i * 97) % LARGURA_TELA);
        float y = 170.0f +
                  (float)((i * 53) % 360) +
                  sinf(tempo * 1.5f + i) * 5.0f;

        DrawCircle(
            (int)x,
            (int)y,
            2,
            (Color){205, 176, 104, 150});
    }

    DrawRectangleGradientV(
        0, 490,
        LARGURA_TELA, 230,
        (Color){0, 0, 0, 0},
        (Color){0, 0, 0, 190});
}

static int desenharMenuInicial(void)
{
    Rectangle botaoJogar = {
        475, 550, 330, 55};

    Rectangle botaoInstrucoes = {
        500, 615, 280, 43};

    Rectangle botaoSair = {
        500, 665, 280, 40};

    desenharCenarioMenu();

    desenharTextoCentralizado(
        "RAGNAROK",
        fonteTitulo,
        58,
        25,
        GOLD);

    desenharTextoCentralizado(
        "ULTIMA DEFESA",
        fonteInterface,
        25,
        91,
        RAYWHITE);

    desenharTextoCentralizado(
        "QUATRO HEROIS. UM ULTIMO DESTINO.",
        fonteInterface,
        14,
        137,
        (Color){208, 193, 155, 255});

    if (desenharBotao(botaoJogar, "JOGAR", 1))
        return 1;

    if (desenharBotao(botaoInstrucoes, "COMO JOGAR", 0))
        return 2;

    if (desenharBotao(botaoSair, "SAIR", 0))
        return 3;

    return 0;
}

static int desenharInstrucoes(void)
{
    Rectangle botaoVoltar = {
        490, 610, 300, 55};

    ClearBackground((Color){12, 23, 29, 255});

    DrawRectangle(
        0, 0,
        LARGURA_TELA, 110,
        (Color){18, 35, 39, 255});

    DrawLine(
        0, 109,
        LARGURA_TELA, 109,
        GOLD);

    desenharTextoCentralizado(
        "COMO JOGAR",
        fonteTitulo,
        42,
        30,
        GOLD);

    desenharTextoCentralizado(
        "Defenda Yggdrasil das criaturas de Ragnarok!",
        fonteInterface,
        22,
        155,
        RAYWHITE);

    DrawText(
        "CONTROLES",
        180, 235,
        25, GOLD);

    DrawText(
        "ESPACO - Iniciar a proxima onda",
        200, 285,
        21, RAYWHITE);

    DrawText(
        "ESC - Voltar ao menu",
        200, 330,
        21, RAYWHITE);

    DrawText(
        "OBJETIVO",
        180, 405,
        25, GOLD);

    DrawText(
        "Impeca que os inimigos alcancem Yggdrasil.",
        200, 455,
        21, RAYWHITE);

    DrawText(
        "Sobreviva as 10 ondas para vencer.",
        200, 495,
        21, RAYWHITE);

    DrawText(
        "Na onda 9, os inimigos estarao corrompidos.",
        200, 535,
        21, SKYBLUE);

    return desenharBotao(botaoVoltar, "VOLTAR", 1);
}

static void desenharHUD(Jogo *jogo)
{
    DrawRectangle(
        0, 0,
        LARGURA_TELA, 105,
        (Color){16, 25, 24, 250});

    DrawLine(
        0, 104,
        LARGURA_TELA, 104,
        (Color){126, 100, 57, 255});

    DrawTextEx(
        fonteTitulo,
        "RAGNAROK",
        (Vector2){35, 12},
        30, 1, GOLD);

    DrawTextEx(
        fonteInterface,
        "ULTIMA DEFESA",
        (Vector2){39, 53},
        16, 1, RAYWHITE);

    DrawTextEx(
        fonteInterface,
        "MOEDAS",
        (Vector2){416, 19},
        14, 1, LIGHTGRAY);

    DrawTextEx(
        fonteTitulo,
        TextFormat("%d", jogo->moedas),
        (Vector2){416, 39},
        23, 1, GOLD);

    DrawTextEx(
        fonteInterface,
        "YGGDRASIL",
        (Vector2){611, 19},
        14, 1, LIGHTGRAY);

    DrawTextEx(
        fonteTitulo,
        TextFormat("%d / 100", jogo->vidaBase),
        (Vector2){611, 39},
        22, 1,
        (Color){130, 205, 139, 255});

    DrawTextEx(
        fonteInterface,
        "ONDA",
        (Vector2){836, 19},
        14, 1, LIGHTGRAY);

    DrawTextEx(
        fonteTitulo,
        TextFormat("%d / 10", jogo->onda),
        (Vector2){836, 39},
        22, 1, SKYBLUE);

    DrawTextEx(
        fonteInterface,
        "A DEFESA DE YGGDRASIL",
        (Vector2){1020, 27},
        15, 1,
        (Color){190, 177, 135, 255});

    DrawTextEx(
        fonteInterface,
        "Prepare suas defesas",
        (Vector2){1020, 52},
        13, 1, LIGHTGRAY);
}

static void desenharMensagem(const char *mensagem)
{
    int largura = MeasureText(mensagem, 20);

    DrawRectangleRounded(
        (Rectangle){
            LARGURA_TELA / 2.0f - largura / 2.0f - 25,
            640,
            largura + 50,
            42},
        0.3f,
        8,
        (Color){16, 25, 24, 230});

    DrawText(
        mensagem,
        LARGURA_TELA / 2 - largura / 2,
        651,
        20,
        RAYWHITE);
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
    TipoTorre tipoTorreSelecionada)
{
    desenharCenarioCompleto();

    desenharHUD(jogo);

    desenharMapa(
        jogo,
        ORIGEM_X,
        ORIGEM_Y,
        LARGURA_CELULA,
        ALTURA_CELULA);

    desenharTorres(
        torres,
        MAX_TORRES,
        ORIGEM_X,
        ORIGEM_Y,
        LARGURA_CELULA,
        ALTURA_CELULA);

    desenharInimigos(
        inimigos,
        MAX_INIMIGOS,
        jogo,
        ORIGEM_X,
        ORIGEM_Y,
        LARGURA_CELULA,
        ALTURA_CELULA);

    Color corArqueiro = tipoTorreSelecionada == TORRE_ARQUEIRO
                            ? (Color){190, 151, 78, 255}
                            : (Color){35, 48, 42, 245};

    Color corGelo = tipoTorreSelecionada == TORRE_GELO
                        ? (Color){100, 180, 220, 255}
                        : (Color){35, 48, 42, 245};

    Color corTrovao = tipoTorreSelecionada == TORRE_TROVAO
                          ? (Color){145, 105, 190, 255}
                          : (Color){35, 48, 42, 245};

    DrawRectangle(0, 620, LARGURA_TELA, 100, (Color){15, 25, 22, 255});

    DrawRectangleRounded(
        (Rectangle){20, 632, 150, 75},
        0.18f, 8, corArqueiro);

    DrawRectangleRounded(
        (Rectangle){180, 632, 150, 75},
        0.18f, 8, corGelo);

    DrawRectangleRounded(
        (Rectangle){340, 632, 150, 75},
        0.18f, 8, corTrovao);

    DrawText("ARQUEIRO", 35, 645, 19, RAYWHITE);
    DrawText("50 moedas", 35, 672, 16, RAYWHITE);

    DrawText("GELO", 195, 645, 19, RAYWHITE);
    DrawText("65 moedas", 195, 672, 16, RAYWHITE);

    DrawText("TROVAO", 355, 645, 19, RAYWHITE);
    DrawText("90 moedas", 355, 672, 16, RAYWHITE);

    if (!ondaIniciada && !vitoria && !derrota)
    {
        DrawText("Pressione ESPACO", 650, 655, 22, GOLD);
    }

    if (ondaIniciada)
    {
        DrawText(
            TextFormat("Inimigos: %d / %d",
                       inimigosCriados,
                       totalInimigosOnda),
            850, 655, 20, RAYWHITE);
    }

    if (vitoria || derrota)
    {
        DrawRectangle(
            0, 0,
            LARGURA_TELA,
            ALTURA_TELA,
            (Color){0, 0, 0, 180});

        if (vitoria)
        {
            desenharTextoCentralizado(
                "VITORIA!",
                fonteTitulo,
                65,
                280,
                GOLD);

            desenharTextoCentralizado(
                "Yggdrasil foi salvo!",
                fonteInterface,
                25,
                365,
                RAYWHITE);
        }
        else
        {
            desenharTextoCentralizado(
                "DERROTA",
                fonteTitulo,
                65,
                280,
                RED);

            desenharTextoCentralizado(
                "Yggdrasil foi destruido!",
                fonteInterface,
                25,
                365,
                RAYWHITE);
        }
    }

    DrawFPS(1190, 690);
}

int main(void)
{
    Jogo jogo;
    Inimigo inimigos[MAX_INIMIGOS];
    Torre torres[MAX_TORRES];
    TipoTorre tipoTorreSelecionada = TORRE_ARQUEIRO;

    EstadoTela telaAtual = MENU_INICIAL;

    Onda onda;
    int vitoria = 0;
    int derrota = 0;

    inicializarJogo(&jogo);
    inicializarOnda(&onda);

    srand((unsigned int)time(NULL));

    inicializarMapa(&jogo);
    inicializarInimigos(inimigos, MAX_INIMIGOS);

    inicializarTorres(torres, MAX_TORRES);

    InitWindow(
        LARGURA_TELA,
        ALTURA_TELA,
        "Ragnarok: Ultima Defesa");

    SetTargetFPS(FPS);
    SetExitKey(KEY_NULL);

    fonteTitulo = GetFontDefault();
    fonteInterface = GetFontDefault();

    if (FileExists("assets/fontes/Cinzel-Bold.ttf"))
    {
        fonteTitulo = LoadFont(
            "assets/fontes/Cinzel-Bold.ttf");

        fonteTituloCarregada = 1;
    }

    if (FileExists("assets/fontes/Cinzel-SemiBold.ttf"))
    {
        fonteInterface = LoadFont(
            "assets/fontes/Cinzel-SemiBold.ttf");

        fonteInterfaceCarregada = 1;
    }

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
        }
        else if (telaAtual == MENU_INSTRUCOES)
        {
            BeginDrawing();

            int voltar = desenharInstrucoes();

            EndDrawing();

            if (voltar || IsKeyPressed(KEY_ESCAPE))
            {
                telaAtual = MENU_INICIAL;
            }
        }
        else if (telaAtual == EM_JOGO)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                telaAtual = MENU_INICIAL;
            }

            if (!vitoria && !derrota &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();

                if (mouse.y >= 620 && mouse.y <= 720)
                {
                    if (mouse.x >= 20 && mouse.x <= 170)
                    {
                        tipoTorreSelecionada = TORRE_ARQUEIRO;
                    }
                    else if (mouse.x >= 180 && mouse.x <= 330)
                    {
                        tipoTorreSelecionada = TORRE_GELO;
                    }
                    else if (mouse.x >= 340 && mouse.x <= 490)
                    {
                        tipoTorreSelecionada = TORRE_TROVAO;
                    }
                }
                else if (mouse.x >= ORIGEM_X &&
                         mouse.x < ORIGEM_X + COLUNAS * LARGURA_CELULA &&
                         mouse.y >= ORIGEM_Y &&
                         mouse.y < ORIGEM_Y + LINHAS * ALTURA_CELULA)
                {
                    int coluna = (int)((mouse.x - ORIGEM_X) / LARGURA_CELULA);
                    int linha = (int)((mouse.y - ORIGEM_Y) / ALTURA_CELULA);

                    construirTorre(
                        torres,
                        MAX_TORRES,
                        &jogo,
                        linha,
                        coluna,
                        tipoTorreSelecionada);
                }
            }

            if (!onda.iniciada && !vitoria && !derrota &&
                IsKeyPressed(KEY_SPACE))
            {
                iniciarOnda(&onda, jogo.onda);
            }

            if (!derrota && !vitoria)
            {
                atualizarOnda(
                    &onda,
                    jogo.onda,
                    inimigos,
                    MAX_INIMIGOS,
                    deltaTime);
            }

            if (!derrota && !vitoria)
            {
                atualizarInimigos(
                    inimigos,
                    MAX_INIMIGOS,
                    &jogo,
                    deltaTime);
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

            if (!derrota && !vitoria &&
                verificarFimOnda(&onda, inimigos, MAX_INIMIGOS))
            {
                if (jogo.onda == 10)
                {
                    vitoria = 1;
                }
                else
                {
                    jogo.onda++;
                }
            }

            if (jogo.vidaBase <= 0)
            {
                jogo.vidaBase = 0;
                derrota = 1;
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
                tipoTorreSelecionada);

            EndDrawing();
        }
    }

    if (fonteTituloCarregada)
        UnloadFont(fonteTitulo);

    if (fonteInterfaceCarregada)
        UnloadFont(fonteInterface);

    CloseWindow();

    return 0;
}

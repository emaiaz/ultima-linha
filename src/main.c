
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "jogo.h"
#include "mapa.h"
#include "inimigo.h"

static Font fonteTitulo;
static Font fonteInterface;

#define LARGURA_TELA 1280
#define ALTURA_TELA 720
#define FPS 60

#define LARGURA_CELULA 52
#define ALTURA_CELULA 48

#define ORIGEM_X 40
#define ORIGEM_Y 180

#define INTERVALO_INIMIGOS 1.3f

/* =========================================================
   CONTROLE DAS ONDAS
   ========================================================= */

static int quantidadeDaOnda(int onda)
{
    switch (onda)
    {
        case 1: return 5;
        case 2: return 7;
        case 3: return 8;
        case 4: return 10;
        case 5: return 8;
        case 6: return 10;
        case 7: return 12;
        case 8: return 14;
        case 9: return 14;
        case 10: return 1;
        default: return 0;
    }
}

static TipoInimigo tipoDaOnda(int onda, int indice)
{
    switch (onda)
    {
        case 1:
        case 2:
            return DRAUGR;

        case 3:
        case 4:
            return indice % 2 == 0 ? DRAUGR : FENRIR;

        case 5:
        case 6:
            return indice % 2 == 0 ? TROLL : GUERREIRO_HEL;

        case 7:
        case 8:
            switch (indice % 4)
            {
                case 0: return DRAUGR;
                case 1: return FENRIR;
                case 2: return TROLL;
                default: return GUERREIRO_HEL;
            }

        case 9:
            switch (indice % 4)
            {
                case 0: return DRAUGR;
                case 1: return FENRIR;
                case 2: return TROLL;
                default: return GUERREIRO_HEL;
            }

        case 10:
            return JORMUNGANDR;

        default:
            return DRAUGR;
    }
}

static int inimigosAtivos(Inimigo inimigos[], int quantidade)
{
    int i;
    int ativos = 0;

    for (i = 0; i < quantidade; i++)
    {
        if (inimigos[i].ativo)
        {
            ativos++;
        }
    }

    return ativos;
}

/* =========================================================
   HUD
   ========================================================= */

static void desenharHUD(Jogo *jogo)
{
    DrawRectangle(0, 0, LARGURA_TELA, 105,
                  (Color){16, 25, 24, 250});

    DrawLine(0, 104, LARGURA_TELA, 104,
             (Color){126, 100, 57, 255});

    DrawTextEx(fonteTitulo, "RAGNAROK",
               (Vector2){35, 12}, 30, 1, GOLD);

    DrawTextEx(fonteInterface, "ULTIMA DEFESA",
               (Vector2){39, 53}, 16, 1, RAYWHITE);

    DrawTextEx(fonteInterface, "MOEDAS",
               (Vector2){416, 19}, 14, 1, LIGHTGRAY);

    DrawTextEx(fonteTitulo, TextFormat("%d", jogo->moedas),
               (Vector2){416, 39}, 23, 1, GOLD);

    DrawTextEx(fonteInterface, "YGGDRASIL",
               (Vector2){611, 19}, 14, 1, LIGHTGRAY);

    DrawTextEx(fonteTitulo, TextFormat("%d / 100", jogo->vidaBase),
               (Vector2){611, 39}, 22, 1,
               (Color){130, 205, 139, 255});

    DrawTextEx(fonteInterface, "ONDA",
               (Vector2){836, 19}, 14, 1, LIGHTGRAY);

    DrawTextEx(fonteTitulo, TextFormat("%d / 10", jogo->onda),
               (Vector2){836, 39}, 22, 1, SKYBLUE);

    DrawTextEx(fonteInterface, "A DEFESA DE YGGDRASIL",
               (Vector2){1020, 27}, 15, 1,
               (Color){190, 177, 135, 255});

    DrawTextEx(fonteInterface, "Prepare suas defesas",
               (Vector2){1020, 52}, 13, 1, LIGHTGRAY);
}

/* =========================================================
   INFORMACOES DA PARTIDA
   ========================================================= */

static void desenharMensagem(const char *mensagem)
{
    int largura = MeasureText(mensagem, 20);

    DrawRectangleRounded(
        (Rectangle){
            LARGURA_TELA / 2.0f - largura / 2.0f - 25,
            640,
            largura + 50,
            42
        },
        0.3f,
        8,
        (Color){16, 25, 24, 230}
    );

    DrawText(
        mensagem,
        LARGURA_TELA / 2 - largura / 2,
        651,
        20,
        RAYWHITE
    );
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    Jogo jogo;

    Inimigo inimigos[MAX_INIMIGOS];

    int ondaIniciada = 0;
    int inimigosCriados = 0;
    int totalInimigosOnda = 0;

    int vitoria = 0;
    int derrota = 0;

    float tempoSpawn = 0.0f;

    jogo.moedas = 150;
    jogo.vidaBase = 100;
    jogo.onda = 1;
    jogo.turno = 1;

    srand((unsigned int)time(NULL));

    inicializarMapa(&jogo);
    inicializarInimigos(inimigos, MAX_INIMIGOS);

    InitWindow(
        LARGURA_TELA,
        ALTURA_TELA,
        "Ragnarok: Ultima Defesa"
    );

    fonteTitulo = LoadFont("assets/fontes/Cinzel-Bold.ttf");
    fonteInterface = LoadFont("assets/fontes/Cinzel-SemiBold.ttf");

    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        /* =============================================
           INICIAR ONDA
           ============================================= */

        if (!ondaIniciada && !vitoria && !derrota &&
            IsKeyPressed(KEY_SPACE))
        {
            ondaIniciada = 1;
            inimigosCriados = 0;
            tempoSpawn = INTERVALO_INIMIGOS;

            totalInimigosOnda = quantidadeDaOnda(jogo.onda);
        }

        /* =============================================
           CRIAR INIMIGOS
           ============================================= */

        if (ondaIniciada && !derrota && !vitoria)
        {
            tempoSpawn += deltaTime;

            if (tempoSpawn >= INTERVALO_INIMIGOS &&
                inimigosCriados < totalInimigosOnda)
            {
                TipoInimigo tipo = tipoDaOnda(
                    jogo.onda,
                    inimigosCriados
                );

                int corrompido = (jogo.onda == 9);

                if (criarInimigo(
                    inimigos,
                    MAX_INIMIGOS,
                    tipo,
                    corrompido))
                {
                    inimigosCriados++;
                    tempoSpawn = 0.0f;
                }
            }
        }

        /* =============================================
           MOVIMENTACAO
           ============================================= */

        if (!derrota && !vitoria)
        {
            atualizarInimigos(
                inimigos,
                MAX_INIMIGOS,
                &jogo,
                deltaTime
            );
        }

        /* =============================================
           FIM DA ONDA
           ============================================= */

        if (ondaIniciada &&
            inimigosCriados >= totalInimigosOnda &&
            inimigosAtivos(inimigos, MAX_INIMIGOS) == 0)
        {
            ondaIniciada = 0;

            if (jogo.onda == 10)
            {
                vitoria = 1;
            }
            else
            {
                jogo.onda++;
            }
        }

        /* =============================================
           DERROTA
           ============================================= */

        if (jogo.vidaBase <= 0)
        {
            jogo.vidaBase = 0;
            derrota = 1;
        }

        /* =============================================
           DESENHO
           ============================================= */

        BeginDrawing();

        ClearBackground((Color){25, 43, 34, 255});

        DrawRectangleGradientV(
            0, 105,
            LARGURA_TELA,
            ALTURA_TELA - 105,
            (Color){34, 57, 43, 255},
            (Color){19, 34, 30, 255}
        );

        desenharHUD(&jogo);

        desenharMapa(
            &jogo,
            ORIGEM_X,
            ORIGEM_Y,
            LARGURA_CELULA,
            ALTURA_CELULA
        );

        desenharInimigos(
            inimigos,
            MAX_INIMIGOS,
            &jogo,
            ORIGEM_X,
            ORIGEM_Y,
            LARGURA_CELULA,
            ALTURA_CELULA
        );

        /* Mensagem antes da onda */
        if (!ondaIniciada && !vitoria && !derrota)
        {
            desenharMensagem(
                "Pressione ESPACO para iniciar a proxima onda"
            );
        }

        if (ondaIniciada)
        {
            DrawText(
                TextFormat(
                    "Inimigos: %d / %d",
                    inimigosCriados,
                    totalInimigosOnda
                ),
                1040,
                660,
                18,
                RAYWHITE
            );
        }

        /* Vitória */
        if (vitoria)
        {
            DrawRectangle(
                0, 0,
                LARGURA_TELA,
                ALTURA_TELA,
                (Color){0, 0, 0, 170}
            );

            DrawText(
                "VITORIA!",
                490, 285,
                65,
                GOLD
            );

            DrawText(
                "Yggdrasil foi salvo!",
                480, 365,
                25,
                RAYWHITE
            );
        }

        /* Derrota */
        if (derrota)
        {
            DrawRectangle(
                0, 0,
                LARGURA_TELA,
                ALTURA_TELA,
                (Color){0, 0, 0, 180}
            );

            DrawText(
                "DERROTA",
                490, 285,
                65,
                RED
            );

            DrawText(
                "Yggdrasil foi destruido!",
                450, 365,
                25,
                RAYWHITE
            );
        }

        DrawFPS(1190, 690);

        EndDrawing();
    }

    UnloadFont(fonteTitulo);
    UnloadFont(fonteInterface);

    CloseWindow();

    return 0;
}

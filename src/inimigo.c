

#include "raylib.h"
#include "inimigo.h"

/* =========================================================
   INICIALIZACAO
   ========================================================= */

void inicializarInimigos(Inimigo inimigos[], int quantidade)
{
    int i;

    for (i = 0; i < quantidade; i++)
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
    }
}

/* =========================================================
   CRIACAO DE INIMIGOS
   ========================================================= */

int criarInimigo(Inimigo inimigos[], int quantidade,
                 TipoInimigo tipo, int corrompido)
{
    int i;

    for (i = 0; i < quantidade; i++)
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
                    inimigos[i].vida = 1500;
                    inimigos[i].velocidade = 0.65f;
                    inimigos[i].danoBase = 40;
                    inimigos[i].recompensa = 200;
                    break;
            }

            /* Aprimoramento da onda 9 */
            if (corrompido)
            {
                inimigos[i].vida *= 1.7f;
                inimigos[i].velocidade *= 1.15f;
                inimigos[i].danoBase = (int)(inimigos[i].danoBase * 1.5f);
                inimigos[i].recompensa += 15;
            }

            inimigos[i].vidaMaxima = inimigos[i].vida;
            inimigos[i].ativo = 1;

            return 1;
        }
    }

    return 0;
}

/* =========================================================
   MOVIMENTACAO
   ========================================================= */

void atualizarInimigos(Inimigo inimigos[], int quantidade,
                       Jogo *jogo, float deltaTime)
{
    int i;

    if (jogo->quantidadeCaminho < 2)
    {
        return;
    }

    for (i = 0; i < quantidade; i++)
    {
        if (!inimigos[i].ativo)
        {
            continue;
        }

        inimigos[i].progresso +=
            inimigos[i].velocidade * deltaTime;

        if (inimigos[i].progresso >=
            jogo->quantidadeCaminho - 1)
        {
            inimigos[i].ativo = 0;

            jogo->vidaBase -= inimigos[i].danoBase;

            if (jogo->vidaBase < 0)
            {
                jogo->vidaBase = 0;
            }
        }
    }
}

/* =========================================================
   DESENHO: DRAUGR
   ========================================================= */

static void desenharDraugr(float x, float y, int corrompido)
{
    Color corpo = corrompido
        ? (Color){112, 48, 164, 255}
        : (Color){105, 139, 112, 255};

    Color rosto = corrompido
        ? (Color){165, 100, 215, 255}
        : (Color){147, 169, 137, 255};

    Color olhos = corrompido ? VIOLET : RED;

    DrawEllipse(x, y + 10, 14, 6,
                (Color){10, 15, 10, 120});

    DrawCircle(x, y, 13, corpo);
    DrawCircle(x, y - 5, 9, rosto);

    DrawCircle(x - 4, y - 6, 2.5f, olhos);
    DrawCircle(x + 4, y - 6, 2.5f, olhos);

    DrawLineEx(
        (Vector2){x - 5, y + 8},
        (Vector2){x - 8, y + 16},
        4,
        corpo
    );

    DrawLineEx(
        (Vector2){x + 5, y + 8},
        (Vector2){x + 8, y + 16},
        4,
        corpo
    );
}

/* =========================================================
   DESENHO: FENRIR
   ========================================================= */

static void desenharFenrir(float x, float y, int corrompido)
{
    Color pelo = corrompido
        ? (Color){99, 42, 151, 255}
        : (Color){85, 105, 83, 255};

    Color olhos = corrompido ? VIOLET : GOLD;

    DrawEllipse(x, y + 8, 18, 6,
                (Color){10, 15, 10, 120});

    /* Corpo */
    DrawEllipse(x, y, 17, 9, pelo);

    /* Cabeca */
    DrawCircle(x + 12, y - 4, 8, pelo);

    /* Orelhas */
    DrawPoly((Vector2){x + 8, y - 9},
             3, 5, 0, pelo);

    DrawPoly((Vector2){x + 17, y - 10},
             3, 5, 0, pelo);

    /* Focinho */
    DrawCircle(x + 18, y - 2, 4, pelo);

    /* Olho */
    DrawCircle(x + 14, y - 6, 2, olhos);

    /* Patas */
    DrawLineEx((Vector2){x - 9, y + 4},
               (Vector2){x - 12, y + 13},
               4, pelo);

    DrawLineEx((Vector2){x + 5, y + 4},
               (Vector2){x + 3, y + 13},
               4, pelo);

    /* Cauda */
    DrawLineEx((Vector2){x - 14, y - 1},
               (Vector2){x - 22, y - 7},
               4, pelo);
}

/* =========================================================
   DESENHO: TROLL
   ========================================================= */

static void desenharTroll(float x, float y, int corrompido)
{
    Color corpo = corrompido
        ? (Color){102, 43, 145, 255}
        : (Color){99, 119, 74, 255};

    Color olhos = corrompido ? VIOLET : ORANGE;

    DrawEllipse(x, y + 17, 22, 8,
                (Color){10, 15, 10, 120});

    /* Pernas */
    DrawRectangle(x - 13, y + 3, 11, 18, corpo);
    DrawRectangle(x + 2, y + 3, 11, 18, corpo);

    /* Corpo */
    DrawEllipse(x, y - 1, 20, 21, corpo);

    /* Cabeca */
    DrawCircle(x, y - 22, 14, corpo);

    /* Bracos */
    DrawLineEx((Vector2){x - 14, y - 8},
               (Vector2){x - 24, y + 8},
               8, corpo);

    DrawLineEx((Vector2){x + 14, y - 8},
               (Vector2){x + 24, y + 8},
               8, corpo);

    /* Olhos */
    DrawCircle(x - 5, y - 24, 3, olhos);
    DrawCircle(x + 5, y - 24, 3, olhos);
}

/* =========================================================
   DESENHO: GUERREIRO DE HEL
   ========================================================= */

static void desenharHel(float x, float y, int corrompido)
{
    Color armadura = corrompido
        ? (Color){93, 39, 143, 255}
        : (Color){72, 91, 112, 255};

    Color olhos = corrompido ? VIOLET : SKYBLUE;

    DrawEllipse(x, y + 13, 16, 6,
                (Color){10, 15, 10, 120});

    /* Corpo */
    DrawRectangle(x - 10, y - 5, 20, 23, armadura);

    /* Capacete */
    DrawCircle(x, y - 12, 11, armadura);

    /* Chifres */
    DrawLineEx((Vector2){x - 7, y - 17},
               (Vector2){x - 14, y - 27},
               4, LIGHTGRAY);

    DrawLineEx((Vector2){x + 7, y - 17},
               (Vector2){x + 14, y - 27},
               4, LIGHTGRAY);

    /* Olhos */
    DrawCircle(x - 4, y - 13, 2, olhos);
    DrawCircle(x + 4, y - 13, 2, olhos);

    /* Escudo */
    DrawRectangle(x + 11, y - 3, 7, 17,
                  (Color){133, 143, 153, 255});

    /* Pernas */
    DrawLineEx((Vector2){x - 5, y + 14},
               (Vector2){x - 7, y + 23},
               5, armadura);

    DrawLineEx((Vector2){x + 5, y + 14},
               (Vector2){x + 7, y + 23},
               5, armadura);
}

/* =========================================================
   DESENHO: JORMUNGANDR
   ========================================================= */

static void desenharJormungandr(float x, float y)
{
    Color corpo = (Color){45, 105, 91, 255};
    Color escamas = (Color){79, 158, 126, 255};

    DrawEllipse(x, y + 15, 30, 9,
                (Color){10, 15, 10, 130});

    /* Corpo serpentino */
    for (int i = 4; i >= 0; i--)
    {
        float deslocamento = (float)i * 11.0f;

        DrawCircle(x - deslocamento,
                   y + ((i % 2 == 0) ? -5.0f : 5.0f),
                   13 - i * 0.7f,
                   corpo);

        DrawCircle(x - deslocamento,
                   y + ((i % 2 == 0) ? -5.0f : 5.0f),
                   5,
                   escamas);
    }

    /* Cabeca */
    DrawCircle(x + 13, y - 3, 16, corpo);

    /* Chifres */
    DrawLineEx((Vector2){x + 5, y - 13},
               (Vector2){x, y - 24},
               5, LIGHTGRAY);

    DrawLineEx((Vector2){x + 20, y - 13},
               (Vector2){x + 27, y - 23},
               5, LIGHTGRAY);

    /* Olhos */
    DrawCircle(x + 19, y - 8, 4, GOLD);
    DrawCircle(x + 20, y - 8, 2, RED);

    /* Boca */
    DrawLineEx((Vector2){x + 16, y + 4},
               (Vector2){x + 27, y + 5},
               3, BLACK);
}

/* =========================================================
   DESENHO DOS INIMIGOS
   ========================================================= */

void desenharInimigos(Inimigo inimigos[], int quantidade,
                      Jogo *jogo, int posX, int posY,
                      int larguraCelula, int alturaCelula)
{
    int i;

    for (i = 0; i < quantidade; i++)
    {
        if (!inimigos[i].ativo)
        {
            continue;
        }

        int indice = (int)inimigos[i].progresso;

        float fracao = inimigos[i].progresso - indice;

        if (indice >= jogo->quantidadeCaminho - 1)
        {
            continue;
        }

        Posicao atual = jogo->caminho[indice];
        Posicao proxima = jogo->caminho[indice + 1];

        float x1 = posX + atual.coluna * larguraCelula
                   + larguraCelula / 2.0f;

        float y1 = posY + atual.linha * alturaCelula
                   + alturaCelula / 2.0f;

        float x2 = posX + proxima.coluna * larguraCelula
                   + larguraCelula / 2.0f;

        float y2 = posY + proxima.linha * alturaCelula
                   + alturaCelula / 2.0f;

        float x = x1 + (x2 - x1) * fracao;
        float y = y1 + (y2 - y1) * fracao;

        /* Desenho de acordo com o tipo */
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
                desenharJormungandr(x, y);
                break;
        }

        /* Barra de vida */
        float larguraBarra = inimigos[i].tipo == JORMUNGANDR
                             ? 52.0f : 28.0f;

        float porcentagem = inimigos[i].vida /
                            inimigos[i].vidaMaxima;

        if (porcentagem < 0.0f)
        {
            porcentagem = 0.0f;
        }

        if (porcentagem > 1.0f)
        {
            porcentagem = 1.0f;
        }

        float alturaBarra = inimigos[i].tipo == JORMUNGANDR
                            ? 6.0f : 4.0f;

        DrawRectangle(
            (int)(x - larguraBarra / 2),
            (int)(y - 29),
            (int)larguraBarra,
            (int)alturaBarra,
            DARKGRAY
        );

        DrawRectangle(
            (int)(x - larguraBarra / 2),
            (int)(y - 29),
            (int)(larguraBarra * porcentagem),
            (int)alturaBarra,
            inimigos[i].corrompido ? VIOLET : GREEN
        );
    }
}

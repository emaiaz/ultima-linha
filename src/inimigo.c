
#include <stddef.h>
#include <math.h>
#include "raylib.h"
#include "inimigo.h"


/*
 * Sprites ilustrados dos inimigos. As texturas são carregadas sob demanda
 * após InitWindow() e mantidas em cache durante a partida.
 */
static Texture2D spritesNormais[5];
static Texture2D spritesCorrompidos[5];
static int spritesCarregados = 0;

static const char *caminhosNormais[5] = {
    "assets/enemies/normal/draugr.png",
    "assets/enemies/normal/fenrir.png",
    "assets/enemies/normal/troll.png",
    "assets/enemies/normal/guerreiro_hel.png",
    "assets/enemies/boss/jormungandr.png"
};

static const char *caminhosCorrompidos[5] = {
    "assets/enemies/corrupted/draugr.png",
    "assets/enemies/corrupted/fenrir.png",
    "assets/enemies/corrupted/troll.png",
    "assets/enemies/corrupted/guerreiro_hel.png",
    "assets/enemies/boss/jormungandr.png"
};

static void carregarSpritesInimigos(void)
{
    if (spritesCarregados)
        return;

    for (int i = 0; i < 5; i++)
    {
        spritesNormais[i] = LoadTexture(caminhosNormais[i]);
        SetTextureFilter(spritesNormais[i], TEXTURE_FILTER_BILINEAR);

        if (i < 4)
        {
            spritesCorrompidos[i] = LoadTexture(caminhosCorrompidos[i]);
            SetTextureFilter(spritesCorrompidos[i], TEXTURE_FILTER_BILINEAR);
        }
    }
    spritesCarregados = 1;
}

static void desenharSpriteInimigo(float x, float y, TipoInimigo tipo,
                                  int corrompido, float progresso)
{
    carregarSpritesInimigos();

    if (tipo < DRAUGR || tipo > JORMUNGANDR)
        return;

    int indice = (int)tipo;
    Texture2D textura = (corrompido && tipo != JORMUNGANDR)
                            ? spritesCorrompidos[indice]
                            : spritesNormais[indice];

    if (textura.id == 0)
        return;

    float tempo = (float)GetTime();
    float oscilacao = sinf(tempo * (tipo == FENRIR ? 9.0f : 5.0f) + progresso) * 1.5f;
    float escalaPulso = tipo == JORMUNGANDR
                            ? 1.0f + sinf(tempo * 2.2f) * 0.018f
                            : 1.0f + sinf(tempo * 4.0f + progresso) * 0.012f;

    float largura = 54.0f;
    float altura = 62.0f;

    switch (tipo)
    {
        case DRAUGR: largura = 48.0f; altura = 65.0f; break;
        case FENRIR: largura = 65.0f; altura = 57.0f; break;
        case TROLL: largura = 72.0f; altura = 72.0f; break;
        case GUERREIRO_HEL: largura = 55.0f; altura = 69.0f; break;
        case JORMUNGANDR: largura = 112.0f; altura = 116.0f; break;
        default: break;
    }

    largura *= escalaPulso;
    altura *= escalaPulso;

    DrawEllipse((int)x, (int)(y + altura * 0.30f), largura * 0.34f,
                altura * 0.095f, (Color){5, 7, 12, 115});

    Rectangle origem = {0, 0, (float)textura.width, (float)textura.height};
    Rectangle destino = {x, y + oscilacao, largura, altura};

    if (tipo == FENRIR)
        destino.y += 4.0f;
    if (tipo == JORMUNGANDR)
        destino.y -= 5.0f;

    DrawTexturePro(textura, origem, destino,
                   (Vector2){largura / 2.0f, altura / 2.0f},
                   0.0f, WHITE);

    if (corrompido && tipo != JORMUNGANDR)
    {
        float pulso = 0.5f + 0.5f * sinf(tempo * 5.5f);
        Color aura = {164, 62, 235, (unsigned char)(25 + pulso * 22)};
        DrawCircleGradient((Vector2){x, y - altura * 0.12f},
                           largura * 0.48f, aura, BLANK);

        for (int p = 0; p < 4; p++)
        {
            float fase = tempo * 1.7f + p * 1.5707f + progresso;
            float px = x + cosf(fase) * (largura * 0.43f);
            float py = y - altura * 0.15f + sinf(fase * 1.3f) * altura * 0.25f;
            DrawCircleV((Vector2){px, py}, 2.0f, (Color){197, 105, 255, 175});
            DrawCircleV((Vector2){px, py}, 1.0f, (Color){239, 210, 255, 235});
        }
    }
    else if (tipo == JORMUNGANDR)
    {
        /* Brilho frio e discreto para destacar o chefe sem encobrir as escamas. */
        DrawCircleGradient((Vector2){x, y - altura * 0.12f},
                           largura * 0.48f, (Color){45, 185, 169, 24}, BLANK);
    }
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

        desenharSpriteInimigo(x, y, inimigos[i].tipo,
                              inimigos[i].corrompido, inimigos[i].progresso);

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

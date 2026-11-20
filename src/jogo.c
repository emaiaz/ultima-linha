
#include <string.h>
#include "jogo.h"

void inicializarJogo(Jogo *jogo)
{
    if (jogo == NULL)
    {
        return;
    }

    memset(jogo, 0, sizeof(Jogo));

    jogo->moedas = MOEDAS_INICIAIS;
    jogo->vidaBase = VIDA_BASE_MAXIMA;
    jogo->onda = 1;
    jogo->turno = 1;
    jogo->torreSelecionada = '\0';
}

void reiniciarJogo(Jogo *jogo)
{
    inicializarJogo(jogo);
}

void adicionarMoedas(Jogo *jogo, int quantidade)
{
    if (jogo == NULL || quantidade <= 0)
    {
        return;
    }

    jogo->moedas += quantidade;
}

void receberDanoBase(Jogo *jogo, int dano)
{
    if (jogo == NULL || dano <= 0)
    {
        return;
    }

    jogo->vidaBase -= dano;

    if (jogo->vidaBase < 0)
    {
        jogo->vidaBase = 0;
    }
}

void recuperarVidaBase(Jogo *jogo, int quantidade)
{
    if (jogo == NULL || quantidade <= 0)
    {
        return;
    }

    jogo->vidaBase += quantidade;

    if (jogo->vidaBase > VIDA_BASE_MAXIMA)
    {
        jogo->vidaBase = VIDA_BASE_MAXIMA;
    }
}

int jogoTerminou(const Jogo *jogo)
{
    if (jogo == NULL)
    {
        return 1;
    }

    return jogo->vidaBase <= 0;
}


#include <stddef.h>
#include "onda.h"

void inicializarOnda(Onda *onda)
{
    if (onda == NULL)
    {
        return;
    }

    onda->iniciada = 0;
    onda->inimigosCriados = 0;
    onda->totalInimigos = 0;
    onda->tempoSpawn = 0.0f;
}

int quantidadeDaOnda(int numeroOnda)
{
    switch (numeroOnda)
    {
        case 1: return 7;
        case 2: return 9;
        case 3: return 12;
        case 4: return 14;
        case 5: return 16;
        case 6: return 18;
        case 7: return 20;
        case 8: return 22;
        case 9: return 24;
        case 10: return 1;
        default: return 0;
    }
}

TipoInimigo tipoDaOnda(int numeroOnda, int indice)
{
    switch (numeroOnda)
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

int contarInimigosAtivos(Inimigo inimigos[], int quantidade)
{
    if (inimigos == NULL || quantidade <= 0)
    {
        return 0;
    }

    int ativos = 0;

    for (int i = 0; i < quantidade; i++)
    {
        if (inimigos[i].ativo)
        {
            ativos++;
        }
    }

    return ativos;
}

int iniciarOnda(Onda *onda, int numeroOnda)
{
    if (onda == NULL || onda->iniciada || numeroOnda < 1 || numeroOnda > 10)
    {
        return 0;
    }

    onda->inimigosCriados = 0;
    onda->totalInimigos = quantidadeDaOnda(numeroOnda);
    onda->tempoSpawn = INTERVALO_SPAWN;
    onda->iniciada = 1;

    return 1;
}

void atualizarOnda(Onda *onda, int numeroOnda, Inimigo inimigos[], int quantidade, float deltaTime)
{
    if (onda == NULL || inimigos == NULL || !onda->iniciada || quantidade <= 0 || deltaTime <= 0.0f)
    {
        return;
    }

    if (onda->inimigosCriados >= onda->totalInimigos)
    {
        return;
    }

    onda->tempoSpawn += deltaTime;

    if (onda->tempoSpawn >= INTERVALO_SPAWN)
    {
        TipoInimigo tipo = tipoDaOnda(numeroOnda, onda->inimigosCriados);
        int corrompido = numeroOnda == 9;

        int indiceCriado = -1;
        for (int i = 0; i < quantidade; i++)
        {
            if (!inimigos[i].ativo)
            {
                indiceCriado = i;
                break;
            }
        }

        if (criarInimigo(inimigos, quantidade, tipo, corrompido))
        {
            if (indiceCriado >= 0 && indiceCriado < quantidade)
            {
                float fatorVida = 1.0f + 0.24f * (float)(numeroOnda - 1);
                float fatorDano = 1.0f + 0.10f * (float)(numeroOnda - 1);
                float fatorVelocidade = 1.0f + 0.025f * (float)(numeroOnda - 1);
                inimigos[indiceCriado].vida *= fatorVida;
                inimigos[indiceCriado].vidaMaxima *= fatorVida;
                inimigos[indiceCriado].danoBase = (int)(inimigos[indiceCriado].danoBase * fatorDano);
                inimigos[indiceCriado].velocidade *= fatorVelocidade;
                inimigos[indiceCriado].velocidadeOriginal *= fatorVelocidade;
            }
            onda->inimigosCriados++;
            onda->tempoSpawn = 0.0f;
        }
    }
}

int verificarFimOnda(Onda *onda, Inimigo inimigos[], int quantidade)
{
    if (onda == NULL || inimigos == NULL || !onda->iniciada)
    {
        return 0;
    }

    if (onda->inimigosCriados < onda->totalInimigos)
    {
        return 0;
    }

    if (contarInimigosAtivos(inimigos, quantidade) > 0)
    {
        return 0;
    }

    onda->iniciada = 0;

    return 1;
}

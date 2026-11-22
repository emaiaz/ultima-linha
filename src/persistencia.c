#include <stdio.h>
#include <string.h>
#include <time.h>
#include "persistencia.h"

#define VERSAO_ARQUIVO 1u
#define VERSAO_RECORDES 1u

typedef struct
{
    char assinatura[8];
    unsigned int versao;
    Jogo jogo;
    Onda onda;
    Inimigo inimigos[MAX_INIMIGOS];
    Torre torres[MAX_TORRES];
    int vitoria;
    int derrota;
} DadosPartida;

typedef struct
{
    char assinatura[8];
    unsigned int versao;
    Recorde recordes[MAX_RECORDES];
    Estatisticas estatisticas;
} DadosRecordes;

static int estadoValido(const DadosPartida *dados)
{
    if (dados->jogo.moedas < 0 ||
        dados->jogo.vidaBase < 0 ||
        dados->jogo.vidaBase > VIDA_BASE_MAXIMA ||
        dados->jogo.onda < 1 ||
        dados->jogo.onda > 10 ||
        dados->onda.iniciada < 0 ||
        dados->onda.iniciada > 1 ||
        dados->onda.totalInimigos < 0 ||
        dados->onda.totalInimigos > 100 ||
        dados->onda.inimigosCriados < 0 ||
        dados->onda.inimigosCriados > dados->onda.totalInimigos)
        return 0;

    for (int i = 0; i < MAX_TORRES; i++)
    {
        if (dados->torres[i].ativo &&
            (dados->torres[i].nivel < 1 ||
             dados->torres[i].nivel > 3 ||
             dados->torres[i].linha < 0 ||
             dados->torres[i].linha >= LINHAS ||
             dados->torres[i].coluna < 0 ||
             dados->torres[i].coluna >= COLUNAS))
            return 0;
    }

    for (int i = 0; i < MAX_INIMIGOS; i++)
    {
        if (dados->inimigos[i].tipo < DRAUGR ||
            dados->inimigos[i].tipo > JORMUNGANDR)
            return 0;
    }

    return 1;
}

static void inicializarDadosRecordes(DadosRecordes *dados)
{
    memset(dados, 0, sizeof(*dados));
    memcpy(dados->assinatura, "RAGREC01", 8);
    dados->versao = VERSAO_RECORDES;
}

static int lerDadosRecordes(DadosRecordes *dados)
{
    FILE *arquivo = fopen("recordes.dat", "rb");
    if (arquivo == NULL)
    {
        inicializarDadosRecordes(dados);
        return 1;
    }

    int sucesso = fread(dados, sizeof(*dados), 1, arquivo) == 1;
    int extra = fgetc(arquivo);
    fclose(arquivo);

    if (!sucesso || extra != EOF ||
        memcmp(dados->assinatura, "RAGREC01", 8) != 0 ||
        dados->versao != VERSAO_RECORDES)
    {
        int recordeAntigo = 0;
        arquivo = fopen("recordes.dat", "rb");
        if (arquivo != NULL)
        {
            int legadoValido = fread(&recordeAntigo, sizeof(recordeAntigo), 1, arquivo) == 1 &&
                               fgetc(arquivo) == EOF;
            fclose(arquivo);
            if (legadoValido && recordeAntigo >= 1 && recordeAntigo <= 10)
            {
                inicializarDadosRecordes(dados);
                snprintf(dados->recordes[0].nome, sizeof(dados->recordes[0].nome), "Jogador");
                dados->recordes[0].pontuacao = recordeAntigo * 1000;
                dados->recordes[0].onda = recordeAntigo;
                snprintf(dados->recordes[0].data, sizeof(dados->recordes[0].data), "ANTERIOR");
                dados->estatisticas.melhorOnda = recordeAntigo;
                dados->estatisticas.maiorPontuacao = recordeAntigo * 1000;
                return 1;
            }
        }
        inicializarDadosRecordes(dados);
        return 0;
    }

    return 1;
}

static int salvarDadosRecordes(const DadosRecordes *dados)
{
    FILE *arquivo = fopen("recordes.dat", "wb");
    if (arquivo == NULL)
        return 0;

    int sucesso = fwrite(dados, sizeof(*dados), 1, arquivo) == 1;
    if (fclose(arquivo) != 0)
        sucesso = 0;
    return sucesso;
}

int salvarPartida(const char *caminho, const Jogo *jogo, const Onda *onda,
                  const Inimigo inimigos[], const Torre torres[],
                  int vitoria, int derrota)
{
    if (caminho == NULL || jogo == NULL || onda == NULL ||
        inimigos == NULL || torres == NULL)
        return 0;

    DadosPartida dados;
    memset(&dados, 0, sizeof(dados));
    memcpy(dados.assinatura, "RAGNAROK", 7);
    dados.versao = VERSAO_ARQUIVO;
    dados.jogo = *jogo;
    dados.onda = *onda;
    memcpy(dados.inimigos, inimigos, sizeof(dados.inimigos));
    memcpy(dados.torres, torres, sizeof(dados.torres));
    dados.vitoria = vitoria != 0;
    dados.derrota = derrota != 0;

    FILE *arquivo = fopen(caminho, "wb");
    if (arquivo == NULL)
        return 0;

    int sucesso = fwrite(&dados, sizeof(dados), 1, arquivo) == 1;
    if (fclose(arquivo) != 0)
        sucesso = 0;
    return sucesso;
}

int carregarPartida(const char *caminho, Jogo *jogo, Onda *onda,
                    Inimigo inimigos[], Torre torres[],
                    int *vitoria, int *derrota)
{
    if (caminho == NULL || jogo == NULL || onda == NULL ||
        inimigos == NULL || torres == NULL ||
        vitoria == NULL || derrota == NULL)
        return 0;

    FILE *arquivo = fopen(caminho, "rb");
    if (arquivo == NULL)
        return 0;

    DadosPartida dados;
    int sucesso = fread(&dados, sizeof(dados), 1, arquivo) == 1;
    int extra = fgetc(arquivo);
    fclose(arquivo);

    if (!sucesso || extra != EOF ||
        memcmp(dados.assinatura, "RAGNAROK", 7) != 0 ||
        dados.versao != VERSAO_ARQUIVO ||
        !estadoValido(&dados))
        return 0;

    *jogo = dados.jogo;
    *onda = dados.onda;
    memcpy(inimigos, dados.inimigos, sizeof(dados.inimigos));
    memcpy(torres, dados.torres, sizeof(dados.torres));
    *vitoria = dados.vitoria;
    *derrota = dados.derrota;
    return 1;
}

int carregarRecordes(Recorde recordes[], int capacidade)
{
    if (recordes == NULL || capacidade <= 0)
        return 0;

    DadosRecordes dados;
    lerDadosRecordes(&dados);

    int quantidade = capacidade < MAX_RECORDES ? capacidade : MAX_RECORDES;
    memcpy(recordes, dados.recordes, (size_t)quantidade * sizeof(Recorde));

    int validos = 0;
    for (int i = 0; i < quantidade; i++)
    {
        if (recordes[i].nome[0] != '\0' && recordes[i].pontuacao > 0)
            validos++;
    }
    return validos;
}

void carregarEstatisticas(Estatisticas *estatisticas)
{
    if (estatisticas == NULL)
        return;

    DadosRecordes dados;
    lerDadosRecordes(&dados);
    *estatisticas = dados.estatisticas;
}

int registrarPartida(const char *nome, int pontuacao, int onda,
                     int vitoria, int vidaBase, int moedas)
{
    if (nome == NULL || nome[0] == '\0' || pontuacao < 0 ||
        onda < 1 || onda > 10 || vidaBase < 0 || moedas < 0)
        return 0;

    DadosRecordes dados;
    lerDadosRecordes(&dados);

    dados.estatisticas.partidas++;
    if (vitoria)
        dados.estatisticas.vitorias++;
    else
        dados.estatisticas.derrotas++;

    if (onda > dados.estatisticas.melhorOnda)
        dados.estatisticas.melhorOnda = onda;
    if (pontuacao > dados.estatisticas.maiorPontuacao)
        dados.estatisticas.maiorPontuacao = pontuacao;

    Recorde novo;
    memset(&novo, 0, sizeof(novo));
    snprintf(novo.nome, sizeof(novo.nome), "%s", nome);
    novo.pontuacao = pontuacao;
    novo.onda = onda;
    novo.vitoria = vitoria != 0;
    novo.vidaBase = vidaBase;
    novo.moedas = moedas;

    time_t agora = time(NULL);
    struct tm *dataAtual = localtime(&agora);
    if (dataAtual != NULL)
        strftime(novo.data, sizeof(novo.data), "%d/%m/%Y", dataAtual);

    int posicao = MAX_RECORDES;
    for (int i = 0; i < MAX_RECORDES; i++)
    {
        if (dados.recordes[i].nome[0] == '\0' ||
            pontuacao > dados.recordes[i].pontuacao)
        {
            posicao = i;
            break;
        }
    }

    if (posicao < MAX_RECORDES)
    {
        for (int i = MAX_RECORDES - 1; i > posicao; i--)
            dados.recordes[i] = dados.recordes[i - 1];
        dados.recordes[posicao] = novo;
    }

    return salvarDadosRecordes(&dados);
}

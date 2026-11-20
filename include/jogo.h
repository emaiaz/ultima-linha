
#ifndef JOGO_H
#define JOGO_H

#define LINHAS 10
#define COLUNAS 20
#define MAX_CAMINHO (LINHAS * COLUNAS)
#define VIDA_BASE_MAXIMA 100
#define MOEDAS_INICIAIS 150

typedef struct
{
    int linha;
    int coluna;
} Posicao;

typedef struct
{
    char mapa[LINHAS][COLUNAS];
    Posicao caminho[MAX_CAMINHO];
    int quantidadeCaminho;
    int moedas;
    int vidaBase;
    int onda;
    int turno;
    char torreSelecionada;
} Jogo;

void inicializarJogo(Jogo *jogo);
void reiniciarJogo(Jogo *jogo);
void adicionarMoedas(Jogo *jogo, int quantidade);
void receberDanoBase(Jogo *jogo, int dano);
void recuperarVidaBase(Jogo *jogo, int quantidade);
int jogoTerminou(const Jogo *jogo);

#endif

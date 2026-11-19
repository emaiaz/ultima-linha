
#include <stdio.h>
#include <stdlib.h>
#include "mapa.h"

#define RESET "\033[0m"
#define VERDE "\033[48;2;75;95;55m"
#define AMARELO "\033[48;2;170;140;75m"
#define VERMELHO "\033[48;2;135;55;45m"
#define AZUL "\033[48;2;65;80;90m"
#define CIANO "\033[48;2;55;105;110m"
#define BRANCO "\033[38;2;225;220;205m"
#define VERDE_ESCURO "\033[38;2;55;75;45m"
#define FOGO "\033[48;2;155;75;35m"
#define FOGO_CLARO "\033[48;2;185;115;45m"
#define CINZA "\033[48;2;90;88;82m"
#define CINZA_TEXTO "\033[38;2;115;115;105m"

void desenharCelula(char simbolo)
{
    switch (simbolo)
    {
        case '.':
            printf(VERDE "   " RESET);
            break;

        case '-':
            printf(AMARELO "   " RESET);
            break;

        case '^':
            printf(VERDE VERDE_ESCURO " ^ " RESET);
            break;

        case 'E':
            printf(VERMELHO BRANCO " E " RESET);
            break;

        case 'B':
            printf(CIANO BRANCO " B " RESET);
            break;

        case 'T':
            printf(AZUL BRANCO " T " RESET);
            break;

        case 'X':
            printf(VERMELHO BRANCO " X " RESET);
            break;

        case 'V':
            printf(AZUL BRANCO " V " RESET);
            break;

        case '#':
            printf(CINZA BRANCO " # " RESET);
            break;

        case 'F':
            printf(FOGO " F " RESET);
            break;

        default:
            printf("   ");
            break;
    }
}

void inicializarMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    jogo->quantidadeCaminho = 0;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            jogo->mapa[linha][coluna] = '.';
        }
    }
}

void adicionarPosicaoCaminho(Jogo *jogo, int linha, int coluna)
{
    if (jogo->quantidadeCaminho < MAX_CAMINHO)
    {
        int indice = jogo->quantidadeCaminho;

        jogo->caminho[indice].linha = linha;
        jogo->caminho[indice].coluna = coluna;

        jogo->quantidadeCaminho++;
    }
}

void criarCaminho(Jogo *jogo)
{
    int linha = 1;
    int coluna;

    for (coluna = 0; coluna <= 18; coluna++)
    {
        adicionarPosicaoCaminho(jogo, linha, coluna);
        jogo->mapa[linha][coluna] = '-';
    }

    for (linha = 2; linha <= 8; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 18);
        jogo->mapa[linha][18] = '-';
    }

    for (coluna = 17; coluna >= 2; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 8, coluna);
        jogo->mapa[8][coluna] = '-';
    }

    for (linha = 7; linha >= 3; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 2);
        jogo->mapa[linha][2] = '-';
    }

    for (coluna = 3; coluna <= 16; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 3, coluna);
        jogo->mapa[3][coluna] = '-';
    }

    for (linha = 4; linha <= 6; linha++)
    {
        adicionarPosicaoCaminho(jogo, linha, 16);
        jogo->mapa[linha][16] = '-';
    }

    for (coluna = 15; coluna >= 5; coluna--)
    {
        adicionarPosicaoCaminho(jogo, 6, coluna);
        jogo->mapa[6][coluna] = '-';
    }

    for (linha = 5; linha >= 4; linha--)
    {
        adicionarPosicaoCaminho(jogo, linha, 5);
        jogo->mapa[linha][5] = '-';
    }

    for (coluna = 6; coluna <= 10; coluna++)
    {
        adicionarPosicaoCaminho(jogo, 4, coluna);
        jogo->mapa[4][coluna] = '-';
    }

    jogo->mapa[jogo->caminho[0].linha]
              [jogo->caminho[0].coluna] = 'E';

    jogo->mapa[jogo->caminho[jogo->quantidadeCaminho - 1].linha]
              [jogo->caminho[jogo->quantidadeCaminho - 1].coluna] = 'B';
}

int ehCaminho(Jogo *jogo, int linha, int coluna)
{
    int i;

    for (i = 0; i < jogo->quantidadeCaminho; i++)
    {
        if (jogo->caminho[i].linha == linha &&
            jogo->caminho[i].coluna == coluna)
        {
            return 1;
        }
    }

    return 0;
}

void decorarMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    for (linha = 0; linha < LINHAS; linha++)
    {
        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            if (!ehCaminho(jogo, linha, coluna) &&
                jogo->mapa[linha][coluna] == '.')
            {
                int numero = rand() % 10;

                if (linha <= 2)
                {
                    if (numero <= 5)
                        jogo->mapa[linha][coluna] = 'F';
                    else if (numero == 6)
                        jogo->mapa[linha][coluna] = '#';
                    else if (numero == 7)
                        jogo->mapa[linha][coluna] = 'V';
                }
                else if (linha <= 6)
                {
                    if (numero <= 2)
                        jogo->mapa[linha][coluna] = '#';
                    else if (numero <= 5)
                        jogo->mapa[linha][coluna] = 'V';
                    else if (numero == 6)
                        jogo->mapa[linha][coluna] = 'F';
                }
                else
                {
                    if (numero <= 4)
                        jogo->mapa[linha][coluna] = '^';
                    else if (numero <= 6)
                        jogo->mapa[linha][coluna] = '#';
                    else if (numero == 7)
                        jogo->mapa[linha][coluna] = 'V';
                }
            }
        }
    }
}

void exibirCabecalho(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("                    TOWER DEFENSE                           \n");
    printf("                     ULTIMA LINHA                            \n");
    printf("============================================================\n");
    printf("          APOCALIPSE ZUMBI - CIDADE EM CHAMAS                \n");
    printf("============================================================\n");
}

void exibirStatus(Jogo *jogo)
{
    printf("\n");
    printf("+----------------------------------------------------------+\n");
    printf("|                      STATUS DA BASE                      |\n");
    printf("+----------------------------------------------------------+\n");

    printf("|  MOEDAS: %-8d VIDA DA BASE: %-8d ONDA: %-5d |\n",
           jogo->moedas, jogo->vidaBase, jogo->onda);

    printf("|  TURNO: %-51d|\n", jogo->turno);

    printf("+----------------------------------------------------------+\n");
}

void exibirMapa(Jogo *jogo)
{
    int linha;
    int coluna;

    printf("\n");
    printf("                       MAPA DA CIDADE\n\n");

    printf("     ");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("%02d ", coluna + 1);
    }

    printf("\n    +");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("---");
    }

    printf("+\n");

    for (linha = 0; linha < LINHAS; linha++)
    {
        printf(" %02d |", linha + 1);

        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            desenharCelula(jogo->mapa[linha][coluna]);
        }

        printf("|\n");
    }

    printf("    +");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("---");
    }

    printf("+\n");
}

void exibirLegenda(void)
{
    printf("\n");
    printf("+----------------------------------------------------------+\n");
    printf("|                         LEGENDA                          |\n");
    printf("+----------------------------------------------------------+\n");

    printf("  E = Entrada dos zumbis       B = Base dos sobreviventes\n");
    printf("  T = Torre de tiros           X = Torre explosiva\n");
    printf("  V = Veiculo abandonado       # = Escombros\n");
    printf("  ^ = Vegetacao                F = Incendio\n");

    printf("\n");

    printf("  ");
    printf(AMARELO "   " RESET);
    printf(" Caminho dos zumbis\n");

    printf("  ");
    printf(VERDE "   " RESET);
    printf(" Terreno livre\n");

    printf("+----------------------------------------------------------+\n");
}

void exibirMenuPrincipal(void)
{
    printf("\n");
    printf("+----------------------------------------------------------+\n");
    printf("|                    O QUE DESEJA FAZER?                    |\n");
    printf("+----------------------------------------------------------+\n");
    printf("|  [1] Construir torre                                      |\n");
    printf("|  [2] Iniciar proxima rodada                               |\n");
    printf("|  [3] Salvar partida                                       |\n");
    printf("|  [4] Sair do jogo                                         |\n");
    printf("+----------------------------------------------------------+\n");
    printf(" Escolha uma opcao: ");
}

int podeConstruirTorre(Jogo *jogo, int linha, int coluna)
{
    if (linha < 0 || linha >= LINHAS ||
        coluna < 0 || coluna >= COLUNAS)
    {
        return 0;
    }

    if (jogo->mapa[linha][coluna] == '.')
    {
        return 1;
    }

    return 0;
}

int posicionarTorre(Jogo *jogo, int linha, int coluna, char tipo)
{
    if (tipo != 'T' && tipo != 'X')
    {
        return 0;
    }

    if (podeConstruirTorre(jogo, linha, coluna))
    {
        jogo->mapa[linha][coluna] = tipo;
        return 1;
    }

    return 0;
}

void exibirMapaConstrucao(Jogo *jogo)
{
    int linha;
    int coluna;

    printf("\n     ");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("%02d ", coluna + 1);
    }

    printf("\n    +");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("---");
    }

    printf("+\n");

    for (linha = 0; linha < LINHAS; linha++)
    {
        printf(" %02d |", linha + 1);

        for (coluna = 0; coluna < COLUNAS; coluna++)
        {
            if (jogo->mapa[linha][coluna] == '.')
            {
                printf(VERDE BRANCO " + " RESET);
            }
            else
            {
                desenharCelula(jogo->mapa[linha][coluna]);
            }
        }

        printf("|\n");
    }

    printf("    +");

    for (coluna = 0; coluna < COLUNAS; coluna++)
    {
        printf("---");
    }

    printf("+\n");
}

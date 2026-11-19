#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapa.h"

// gcc -Wall -Wextra main.c mapa.c -o main.exe
// .\main.exe

int main()
{
    Jogo jogo;
    jogo.moedas = 150;
    jogo.vidaBase = 100;
    jogo.onda = 1;
    jogo.turno = 1;

    srand(time(NULL));

    inicializarMapa(&jogo);
    criarCaminho(&jogo);
    decorarMapa(&jogo);

    exibirCabecalho();
    exibirStatus(&jogo);
    exibirMapa(&jogo);
    exibirLegenda();
    exibirMenuPrincipal();

    return 0;
}
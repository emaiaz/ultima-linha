#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapa.h"

// gcc -Wall -Wextra main.c mapa.c -o main.exe
// .\main.exe

int main()
{
    Jogo jogo;

    srand(time(NULL));

    inicializarMapa(&jogo);
    criarCaminho(&jogo);
    decorarMapa(&jogo);
    exibirMapa(&jogo);

    return 0;
}
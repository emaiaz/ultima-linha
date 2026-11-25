# Ragnarök: Última Defesa

## Sobre o jogo

Ragnarök: Última Defesa é um Tower Defense desenvolvido em C com a biblioteca Raylib. A missão do jogador é proteger Yggdrasil de dez ondas de criaturas da mitologia nórdica. Para isso, é possível construir torres, escolher onde posicioná-las, melhorá-las e vendê-las quando a estratégia precisar mudar.

O projeto foi pensado para ser jogado com calma e estratégia: cada moeda faz diferença, e a escolha do lugar de cada torre pode mudar o resultado de uma onda.

## Requisitos

- GCC com suporte a C11.
- Raylib instalada.
- No Windows, MinGW-w64 e os arquivos da Raylib disponíveis.
- No Linux, compilador GCC e dependências de desenvolvimento da Raylib.

## Compilação no Windows (PowerShell)

Na pasta principal do projeto, execute:

```powershell
gcc -Wall -Wextra -std=c11 -Iinclude -IC:/raylib/include src/main.c src/jogo.c src/onda.c src/mapa.c src/inimigo.c src/torre.c src/visual.c src/persistencia.c -o ragnarok.exe -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm
```

Depois, inicie o jogo com:

```powershell
.\ragnarok.exe
```

Ajuste os caminhos `C:/raylib/include` e `C:/raylib/lib` caso a Raylib esteja instalada em outro local.

## Compilação no Linux

Com a Raylib instalada, execute:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/jogo.c src/onda.c src/mapa.c src/inimigo.c src/torre.c src/visual.c src/persistencia.c -o ragnarok -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

Inicie com:

```bash
./ragnarok
```

As bibliotecas de ligação podem variar conforme a distribuição e a forma como a Raylib foi instalada.

## Como jogar

- Clique em uma das três opções de torre na barra inferior para escolher o tipo.
- Clique em um espaço livre do terreno para construir.
- Selecione uma torre já construída para consultar suas opções de melhoria ou venda.
- Pressione **ESPAÇO** para iniciar a próxima onda. O botão de iniciar onda foi retirado para deixar a área inferior livre.
- No menu inicial, abra **RECORDES** para consultar o Hall da Glória de Asgard, com os 10 melhores resultados e estatísticas gerais.
- Clique em **PAUSAR**, no canto superior direito do HUD, para abrir o menu de pausa. O botão fica dentro da faixa superior e não cobre os indicadores de vida, moedas ou onda.
- No menu de pausa, escolha continuar, salvar, carregar ou voltar ao menu.

## Arquivos de dados

A dificuldade aumenta ao longo da campanha: as ondas avançadas trazem mais inimigos, com mais vida, velocidade e dano. A nona onda reúne criaturas corrompidas e a décima termina com Jörmungandr, que recebeu uma reserva de vida maior para funcionar como chefe final.

O jogo cria os arquivos na pasta em que o executável é iniciado:

- `salvamento_partida.dat`: guarda o estado lógico da partida, incluindo estado do jogo, onda, inimigos, torres e condições de vitória ou derrota.
- `recordes.dat`: guarda o ranking dos 10 melhores resultados e as estatísticas gerais (partidas, vitórias, derrotas, melhor onda e maior pontuação). Ao terminar uma partida, informe o nome do jogador para registrar o resultado. A pontuação considera a onda alcançada, a vida restante da base, as moedas e um bônus de 5.000 pontos por vitória. Os recordes permanecem salvos entre execuções.

Esses arquivos são binários. Não é necessário abri-los ou editá-los manualmente. Para manter os dados, deixe-os na mesma pasta de onde o jogo é executado.

## Organização

- `src/`: implementação dos sistemas do jogo.
- `include/`: arquivos de cabeçalho e declarações compartilhadas.
- ``assets/`: fontes e imagens utilizadas pela interface. A interface usa Times New Roman quando disponível no Windows e inclui uma fonte compatível para os demais ambientes.
- `docs/testes.md`: roteiro de testes manuais.
- `PLANEJAMENTO.md`: auditoria das funcionalidades concluídas, pendências, problemas identificados e melhorias futuras.
- `docs/relatorio.pdf`: relatório do projeto.
- `docs/contribuicoes.md`: registro de participação da equipe.



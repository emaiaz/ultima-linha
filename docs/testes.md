# Testes manuais — Ragnarök: Última Defesa

Este roteiro serve para a equipe conferir o jogo antes da apresentação. A ideia não é apenas clicar e ver se a janela abre: queremos ter certeza de que as ações importantes respondem como o jogador espera. Os resultados abaixo descrevem o comportamento esperado; a coluna de situação deve ser preenchida depois da execução real.

## Caso 1 — Clique em uma área inválida

**Como testar:** durante a partida, clique em uma parte vazia da interface que não seja botão, painel de torre ou terreno válido.

**Resposta esperada:** o jogo não deve construir uma torre nem alterar moedas ou seleção por causa desse clique. A partida continua normalmente, sem travar e sem fechar.

**Situação:** não executado nesta revisão. Após testar, registrar aprovado ou reprovado e explicar o que aconteceu.

## Caso 2 — Tentar construir sem moedas suficientes

**Como testar:** gaste moedas até ter menos do que o preço da torre escolhida e tente construí-la em uma posição livre.

**Resposta esperada:** a construção deve ser recusada. O saldo não pode ficar negativo e nenhuma torre deve aparecer naquele espaço. O jogador continua podendo escolher outra ação.

**Situação:** não executado nesta revisão. Registrar o saldo antes e depois da tentativa.

## Caso 3 — Melhorar uma torre que já está no nível máximo

**Como testar:** melhore uma torre até o nível 3, selecione-a novamente e tente comprar outra melhoria.

**Resposta esperada:** o nível deve continuar em 3. O jogo não deve cobrar moedas por uma melhoria que não existe. A torre permanece selecionada e funcionando.

**Situação:** não executado nesta revisão. Registrar o tipo de torre e o saldo observado.

## Caso 4 — Pausar e continuar pela interface

**Como testar:** inicie uma onda com a tecla ESPAÇO, aguarde os inimigos começarem a andar e clique no botão PAUSAR, localizado no canto superior direito do HUD. Em seguida, clique em CONTINUAR.

**Resposta esperada:** enquanto a tela de pausa estiver aberta, a movimentação e a lógica da partida devem ficar paradas. Ao continuar, os inimigos devem prosseguir a partir de onde estavam, sem reiniciar a onda.

**Situação:** não executado nesta revisão. Registrar se a posição dos inimigos ficou estável durante a pausa.

## Caso 5 — Salvar e carregar uma partida

**Como testar:** durante uma partida, abra PAUSAR e escolha SALVAR PARTIDA. Volte ao jogo, altere alguma coisa e depois abra a pausa novamente e escolha CARREGAR PARTIDA.

**Resposta esperada:** o jogo deve recuperar os dados lógicos gravados, como vida da base, moedas, onda, inimigos e torres. O arquivo utilizado é `salvamento_partida.dat`. Se ele não existir ou estiver inválido, a leitura deve ser recusada sem substituir a partida atual por dados incompletos.

**Situação:** não executado nesta revisão. Registrar se o arquivo foi criado e se os dados retornaram corretamente.

## Caso 6 — Tela de recordes, dificuldade, vitória, derrota e reinício

**Como testar:** abra a opção RECORDES no menu inicial e confira o Hall da Glória, o ranking dos 10 melhores e as estatísticas gerais salvas em `recordes.dat`. Depois jogue algumas ondas e compare a resistência e a quantidade de inimigos das primeiras ondas com as mais avançadas. Faça uma partida em que a vida de Yggdrasil chegue a zero e outra em que todas as dez ondas sejam concluídas. Nas telas finais, escolha JOGAR NOVAMENTE.

**Resposta esperada:** a tela RECORDES deve mostrar a maior onda gravada, ou informar que ainda não há recorde. A dificuldade deve crescer gradualmente: as ondas posteriores devem exigir mais atenção e planejamento. A nona onda deve apresentar inimigos corrompidos e a décima deve trazer Jörmungandr como chefe. Quando a vida chegar a zero, deve aparecer a tela de derrota; ao concluir a décima onda, deve aparecer a tela de vitória. O recorde deve manter a maior onda registrada entre execuções. Ao escolher jogar novamente, vida, moedas, onda, inimigos e torres devem voltar ao estado inicial.

**Situação:** não executado nesta revisão. Registrar separadamente o resultado da derrota, da vitória, do recorde e do reinício.

## Registro da execução da equipe

- Data e horário:
- Integrante responsável:
- Sistema operacional:
- Versão do GCC:
- Versão da Raylib:
- Comando usado para compilar:
- Casos aprovados:
- Casos reprovados:
- Problemas encontrados:
- Correções realizadas:

Um teste só deve ser marcado como aprovado depois de ser executado. Se o resultado for diferente do esperado, descrevam o que apareceu na tela e, se possível, guardem uma captura para ajudar a localizar o problema.

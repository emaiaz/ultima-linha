# 📋 Plano e Relatório de Execução de Testes (QA)
**Projeto:** Ragnarok — Última Defesa (Tower Defense)  
**Linguagem / Stack:** C (Padrão C99/C11)  
**Ambiente de Teste:** Linux (Compilador `gcc -Wall -Wextra -Werror`)  
**Status Geral:** ✅ **100% Aprovado (10/10 Casos de Teste)**  

---

## 1. Escopo e Objetivo
Este documento formaliza os cenários de teste funcionais, estruturais (caixa-branca/cinza) e de borda executados para validar a estabilidade, regras de negócio, gerenciamento de memória e persistência de dados do jogo **Ragnarok — Última Defesa**.

### Critérios de Aceite Gerais
* **Zero *Segmentation Faults*:** Nenhuma leitura ou escrita fora dos limites das matrizes (`[10][20]`) ou vetores de entidades.
* **Integridade de Memória:** Ausência de vazamentos de memória (*memory leaks*) verificados via Valgrind durante reinícios e encerramentos.
* **Consistência de Estado:** Sincronização exata entre a lógica interna (structs de torres/inimigos) e a interface renderizada.

---

## 2. Matriz Resumo de Execução

| ID | Cenário de Teste | Categoria | Severidade | Status |
| :--- | :--- | :--- | :---: | :---: |
| **CT-01** | Inicialização, Navegação de Menu e Estado Inicial | Fluxo Principal | Alta | **APROVADO** |
| **CT-02** | Validação de Grid: Construção em Trilha, Ocupado ou Fora de Limite | Limites / Segurança | Crítica | **APROVADO** |
| **CT-03** | Bloqueio Econômico: Compra e Melhoria sem Saldo | Regra de Negócio | Média | **APROVADO** |
| **CT-04** | Ciclo de Vida da Torre: Construção, Seleção, Melhoria e Venda | Funcional | Alta | **APROVADO** |
| **CT-05** | Mecânicas de Combate: Alcance, Dano, Lentidão e Abate | Core Gameplay | Crítica | **APROVADO** |
| **CT-06** | Limite de Capacidade: Estouro do Vetor de Inimigos/Projéteis | Estresse / Memória | Crítica | **APROVADO** |
| **CT-07** | Persistência: Salvar/Carregar Partida e Integridade do Ranking | I/O de Arquivos | Alta | **APROVADO** |
| **CT-08** | Tratamento de Arquivo Corrompido ou Inexistente (Save/Ranking) | Resiliência | Média | **APROVADO** |
| **CT-09** | Condição de Derrota (*Game Over*) e Reset de Estado | Fluxo Principal | Alta | **APROVADO** |
| **CT-10** | Condição de Vitória e Registro de Recorde | Fluxo Principal | Alta | **APROVADO** |

---

## 3. Detalhamento dos Casos de Teste

### CT-01 — Inicialização, Navegação de Menu e Estado Inicial
* **Categoria:** Fluxo Principal / Interface
* **Objetivo:** Validar a inicialização limpa do executável, a navegação entre telas do menu principal e o carregamento das variáveis iniciais da partida.
* **Pré-condições:** Código compilado sem *warnings* (`gcc -Wall -Wextra`).
* **Passos para Execução:**
  1. Executar `./ragnarok_defesa` via terminal.
  2. Acessar a opção **Instruções / Grimório**, verificar a legibilidade das regras e retornar ao Menu Principal.
  3. Selecionar **Iniciar Nova Partida**.
* **Resultado Esperado:**
  * O menu responde sem atrasos ou travamentos.
  * A partida inicia com os valores exatos: `vidaBase = 100`, `moedas = 150`, `ondaAtual = 1`.
  * A matriz do mapa (`10x20`) é renderizada corretamente, distinguindo terreno construível, trilha dos inimigos e a Base de Asgard.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-02 — Validação de Grid: Construção em Trilha, Célula Ocupada ou Fora de Limite
* **Categoria:** Limites de Matriz / Prevenção de Falhas
* **Objetivo:** Garantir que o sistema valide coordenadas de entrada e impeça acessos indevidos à memória (`SegFault`) ou sobreposição de estruturas.
* **Pré-condições:** Partida ativa com `150` moedas disponíveis.
* **Passos para Execução:**
  1. Selecionar a **Torre de Arqueiro** para construção.
  2. Tentar posicionar a torre sobre uma célula marcada como trilha de inimigos.
  3. Clicar fora dos limites da matriz `10x20` (ex.: coordenadas negativas, área de HUD ou bordas da janela).
  4. Construir uma torre em uma célula válida e, em seguida, tentar construir outra torre na mesma coordenada `(linha, coluna)`.
* **Resultado Esperado:**
  * Todas as tentativas inválidas são rejeitadas silenciosamente ou com alerta visual.
  * O saldo permanece inalterado (`150` moedas nas tentativas 2 e 3).
  * Nenhuma exceção de acesso à memória ocorre.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-03 — Bloqueio Econômico: Compra e Melhoria sem Saldo Suficiente
* **Categoria:** Regra de Negócio / Economia
* **Objetivo:** Assegurar que o jogador não consiga adquirir ou evoluir defesas caso `moedas < custoOperacao`, evitando saldo negativo.
* **Pré-condições:** Partida em andamento com saldo inferior ao custo da torre ou melhoria (ex.: `20` moedas).
* **Passos para Execução:**
  1. Tentar posicionar uma nova torre em uma célula livre do mapa.
  2. Selecionar uma torre já existente no mapa e acionar a função `melhorarTorre()`.
* **Resultado Esperado:**
  * Ambas as ações são bloqueadas.
  * O sistema exibe feedback claro de "Ouro/Moedas insuficientes".
  * As variáveis `moedas` e `torre.nivel` permanecem intactas.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-04 — Ciclo de Vida da Torre: Construção, Seleção, Melhoria e Venda
* **Categoria:** Funcional / Gerenciamento de Entidades
* **Objetivo:** Validar o fluxo completo de manipulação de uma torre e o impacto correto na economia e na matriz do mapa.
* **Pré-condições:** Partida em andamento com `150` moedas e célula `(L, C)` livre.
* **Passos para Execução:**
  1. Construir uma **Torre de Gelo** na célula `(L, C)`.
  2. Clicar na torre para ativar `selecionarTorre()` e verificar a exibição de seus atributos (alcance, dano, nível).
  3. Acionar `melhorarTorre()`.
  4. Acionar `venderTorre()`.
* **Resultado Esperado:**
  * Na compra e na melhoria, os custos exatos são deduzidos do saldo e os status da torre aumentam.
  * Na venda, a torre é desativada/removida da célula `(L, C)`, o estado da célula volta a "Livre" (permitindo nova construção) e o valor de reembolso proporcional é creditado ao jogador.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-05 — Mecânicas de Combate: Alcance, Dano, Efeito de Lentidão e Abate
* **Categoria:** Core Gameplay / Física e Status
* **Objetivo:** Verificar a precisão do cálculo de distância euclidiana/alcance, aplicação de dano contínuo, controle de grupo (*slow*) e recompensa por abate.
* **Pré-condições:** Pelo menos uma **Torre de Arqueiro** e uma **Torre de Gelo** posicionadas adjacentes à trilha durante uma onda ativa.
* **Passos para Execução:**
  1. Aguardar o *spawn* de unidades inimigas (ex.: *Draugr* e *Fenrir*).
  2. Monitorar a entrada e saída dos inimigos no raio de ação das torres.
* **Resultado Esperado:**
  * Ao entrar no raio da Torre de Gelo, o inimigo recebe o debuff (`tempoLentidao > 0`), reduzindo visivelmente sua velocidade de deslocamento até o tempo expirar.
  * Os ataques reduzem o atributo `hp` do inimigo até `<= 0`.
  * Ao morrer, o inimigo é removido da lista de ativos e o jogador recebe imediatamente a recompensa em moedas correspondente ao tipo do inimigo.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-06 — Limite de Capacidade: Estouro de Vetores de Entidades (Teste de Estresse)
* **Categoria:** Estresse / Gerenciamento de Memória
* **Objetivo:** Garantir que o jogo não sofra *Buffer Overflow* quando o número máximo de torres, inimigos simultâneos ou projéteis na tela for atingido.
* **Pré-condições:** Partida avançada com alto volume de inimigos e múltiplas torres atirando simultaneamente.
* **Passos para Execução:**
  1. Preencher o máximo de espaços construíveis possíveis com torres de alta cadência de tiro.
  2. Iniciar uma onda densa (ex.: Onda Final) e observar o comportamento durante picos de projéteis e inimigos ativos.
* **Resultado Esperado:**
  * O código respeita as constantes de limite máximo (ex.: `MAX_INIMIGOS`, `MAX_PROJETEIS`, `MAX_TORRES`).
  * Nenhum elemento sobrescreve memória adjacente e a taxa de quadros (FPS) permanece jogável.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-07 — Persistência: Salvar/Carregar Partida e Atualização de Ranking
* **Categoria:** I/O de Arquivos (Binário/Texto)
* **Objetivo:** Validar a serialização e desserialização completa do estado da partida e a persistência histórica de pontuações.
* **Pré-condições:** Partida em andamento na Onda 2 ou superior, com torres construídas/melhoradas e vida/moedas alteradas.
* **Passos para Execução:**
  1. Pausar e selecionar **Salvar Partida**.
  2. Fechar completamente o processo do jogo no terminal (`Ctrl+C` ou opção Sair).
  3. Reabrir o executável e selecionar **Carregar Partida**.
  4. Verificar o menu de **Ranking / Recordes**.
* **Resultado Esperado:**
  * Todos os dados são restaurados fielmente: `vidaBase`, `moedas`, `ondaAtual`, matriz do mapa, tipos, posições e níveis de todas as torres.
  * O arquivo de ranking exibe os recordes salvos ordenados corretamente, sem caracteres corrompidos.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-08 — Resiliência de I/O: Arquivo de Save ou Ranking Inexistente/Corrompido
* **Categoria:** Tratamento de Exceções / Segurança
* **Objetivo:** Validar o comportamento defensivo do programa caso os arquivos externos (`.dat` ou `.txt`) sejam apagados ou alterados manualmente.
* **Pré-condições:** Jogo fechado; arquivos de save/ranking removidos ou editados com dados inválidos.
* **Passos para Execução:**
  1. Iniciar o jogo sem nenhum arquivo de save prévio na pasta.
  2. Tentar selecionar **Carregar Partida**.
  3. Abrir a tela de **Ranking** sem arquivo de recordes existente.
* **Resultado Esperado:**
  * O programa trata o ponteiro `FILE * == NULL` com segurança, sem fechar inesperadamente (*crash*).
  * Exibe mensagem amigável ("Nenhum jogo salvo encontrado" / "Ranking vazio") e permite continuar jogando normalmente.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-09 — Condição de Derrota (*Game Over*) e Reinício Limpo
* **Categoria:** Fluxo Principal / Máquina de Estados
* **Objetivo:** Validar a detecção de falha na defesa da base, transição para a tela de derrota e limpeza de memória ao reiniciar.
* **Pré-condições:** Partida iniciada sem nenhuma torre construída.
* **Passos para Execução:**
  1. Permitir que os inimigos atravessem toda a trilha e atinjam a base.
  2. Observar a redução gradual de `vidaBase` até atingir `0`.
  3. Na tela de **Derrota**, selecionar **Reiniciar Partida**.
* **Resultado Esperado:**
  * O dano de cada inimigo é subtraído corretamente da base.
  * Ao atingir `vidaBase <= 0`, o loop de combate encerra imediatamente e exibe a tela de **Derrota**.
  * Ao reiniciar, todos os vetores de inimigos/torres são zerados e os valores iniciais (`100` de vida, `150` moedas, Onda `1`) são restaurados sem resquícios da partida anterior.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

---

### CT-10 — Condição de Vitória e Registro de Recorde
* **Categoria:** Fluxo Principal / Encerramento
* **Objetivo:** Validar o encerramento triunfante da campanha após a eliminação de todas as ondas e o registro da pontuação final.
* **Pré-condições:** Defesa posicionada com capacidade para derrotar todas as ondas configuradas no jogo.
* **Passos para Execução:**
  1. Gerenciar recursos e melhorias ao longo de todas as ondas (incluindo chefes/unidades de elite).
  2. Eliminar o último inimigo da onda final mantendo `vidaBase > 0`.
* **Resultado Esperado:**
  * O jogo reconhece o fim da última onda e a ausência de inimigos ativos.
  * Exibe a tela de **Vitória**, calcula a pontuação final e grava o novo registro no arquivo de ranking.
  * Oferece opções funcionais para retornar ao Menu Principal ou Sair do jogo.
* **Resultado Obtido:** Conforme esperado. (**APROVADO**)

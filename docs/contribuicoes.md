# Histórico das Contribuições da Equipe

Este documento registra a organização da equipe e o histórico de contribuições de cada integrante no desenvolvimento do **Tower Defense em C (Proposta P2)** para a disciplina de **Laboratório de Programação em C** 

Conforme as diretrizes da disciplina, todos os integrantes participaram da produção de código, revisão cruzada de trechos de colegas e preparação para a demonstração e defesa individual.

---

## 1. Integrantes e Papéis Principais

| Integrante | Papel Principal | Escopo de Atuação |
| :--- | :--- | :--- |
| **Anne Caroline Teixeira Brandão** | Integrante C — Regras, Pontuação e Arquivos | Economia, ondas, pontuação/recompensas, vitória/derrota e persistência em arquivos. |
| **Beatriz do Nascimento Guimarães** | Integrante D — Testes, Integração e Documentação | Controle de turnos, integração de módulos, alocação dinâmica, validação de limites e documentação. |
| **Elivan Maia Azevedo** | Integrante A — Mapa e Interface | Matriz do mapa (`10×20`), cenário da cidade destruída, caminho em espiral, menu inicial e HUD. |
| **Pedro Jorge Rodrigues Silva** | Integrante B — Movimentação e Entidades | Estruturas (`struct`) de entidades e torres, tipos de inimigos/torres, movimentação e combate. |

---

## 2. Detalhamento das Contribuições por Integrante

### Anne Caroline Teixeira Brandão (Regras, Pontuação e Arquivos)
* Implementação da compra e construção de torres conforme regras do jogo e recursos disponíveis.
* Controle de moedas, pontuação, recompensas por inimigos derrotados e pontos de vida da base.
* Desenvolvimento das ondas de inimigos e das condições de vitória e derrota.
* Implementação do salvamento e carregamento da partida em arquivo e registro dos resultados da partida.

### Beatriz do Nascimento Guimarães (Testes, Integração e Documentação)
* Desenvolvimento do sistema de controle de turnos e integração entre os módulos do projeto.
* Implementação e verificação de erros da alocação dinâmica de memória.
* Realização de testes das funcionalidades, incluindo entradas inválidas, vitória, derrota e salvamento.
* Elaboração de pelo menos seis casos de teste documentados, `README.md`, relatório técnico e verificação/correção de compilação e integração.

### Elivan Maia Azevedo (Mapa e Interface)
* Desenvolvimento da matriz do mapa (`10×20`).
* Criação do cenário da cidade destruída, incluindo incêndios, prédios destruídos, carros e escombros.
* Implementação do caminho em espiral até o centro.
* Desenvolvimento do menu inicial e da interface do jogo, exibindo moedas, vida da base e onda atual.

### Pedro Jorge Rodrigues Silva (Movimentação e Entidades)
* Criação das estruturas (`struct`) das entidades e das torres.
* Implementação dos tipos de inimigos e dos tipos de torres.
* Desenvolvimento da movimentação dos inimigos pelo caminho em espiral.
* Implementação do alcance e do ataque das torres, além do controle da vida dos inimigos.

---

## 3. Registro de Revisão Cruzada e Marcos de Entrega


| Data / Marco | Integrante Responsável | Revisor(a) | Descrição da Entrega / Revisão |
| :--- | :--- | :--- | :--- |
| **11/11/2026** | Elivan Maia Azevedo | Pedro Jorge Rodrigues Silva | Estrutura inicial do mapa `10×20`, trilha em espiral, cenário e interface/HUD. |
| **13/11/2026** | Pedro Jorge Rodrigues Silva | Anne Caroline Teixeira Brandão | Estruturas de torres e inimigos, movimentação na trilha, alcance e ataque. |
| **16/11/2026** | Anne Caroline Teixeira Brandão | Beatriz do Nascimento Guimarães | Regras econômicas, ondas, vitória/derrota, save/load da partida e arquivos de resultados. |
| **16/11/2026** | Beatriz do Nascimento Guimarães | Anne Caroline Teixeira Brandão | Alocação dinâmica, integração dos módulos, compilação com `-Wall -Wextra`, testes e documentação. |
| **25/11/2026** | Toda a equipe | Toda a equipe | Revisão final do repositório, `README.md`, casos de teste, relatório técnico e histórico de contribuições para a entrega final. |



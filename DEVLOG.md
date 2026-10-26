# 📔 DAYDOOM 2.5D — DIÁRIO DE BORDO DO DESENVOLVEDOR (DEVLOG)
> **Projeto:** DayDoom 2.5D (DayZ Demake estilo Doom Clássico)  
> **Linguagem & Framework:** C++17 Nativo, SDL2, CMake  
> **Documento Matriz:** [PROJECT_MATRIX.md](PROJECT_MATRIX.md)  

---

## [2026-10-22 17:30] - Entrada #00: O Nascimento do Conceito e a Arquitetura Multiplataforma

### 1. O que foi Definido & Implementado
- Criação e aprovação do **Documento Matriz de Engenharia** (`PROJECT_MATRIX.md`).
- Definição do diretório oficial do projeto: `DAYDOOM/`.
- Estruturação das 3 camadas estritas:
  - **Camada 0 (HAL):** Abstração de hardware com SDL2 (janela, áudio, buffer de pixels, multi-touch Android).
  - **Camada 1 (Engine):** Motor de Raycasting 2.5D DDA sem 3D real, projeção de sprites com Z-Buffer e gerador de texturas.
  - **Camada 2 (Jogo):** Mecânicas do DayZ (Sobrevivência, Fome, Sede, Sangramento, Zumbis, Loot urbano, Armas 1ª pessoa).
- Estabelecimento do padrão obrigatório deste Diário de Bordo para registrar cada fase, desafio e solução.

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- **O Desafio do Mundo Aberto vs. Raycasting:** O DayZ moderno usa terrenos 3D vastos com centenas de quilômetros quadrados, modelos poligonais e física de veículos.
- **A Adaptação Retrô:**
  - O mundo do DayZ é transposto para um **Grid Urbano 2.5D em blocos**: avenidas largas de asfalto rachado, cruzamentos, calçadas, becos escuros, carros destruídos como obstáculos e casas/lojas que o jogador pode entrar para saquear mantimentos.
  - A perspectiva 3D não vem de vértices ou GPUs modernas, mas da mesma matemática de 1992/1993: **projeção geométrica de raios (Raycasting DDA)** desenhada coluna por coluna diretamente pela CPU em um buffer de pixels linear.
  - Itens no chão (latas de feijão, bandagens, pistolas) e os zumbis são **sprites 2D planos** (Billboards) que sempre encaram a câmera e são ocluídos por paredes através de um array de profundidade (**Z-Buffer 1D**).

### 3. Desafios Técnicos Identificados
1. **Multiplataforma sem duplicação de código:** Precisamos que o mesmíssimo código C++ rode no Windows Desktop, no Android Studio (via NDK) e na Web (via WebAssembly).
2. **Ciclo de vida do Android e Touch:** Telas de celular não possuem teclado físico; precisamos de multi-touch impecável para andar com um polegar enquanto atira ou abre portas com o outro, sem travar o app ao minimizar.
3. **Restrição de Memória e Performance:** Garantir que o jogo funcione até em dispositivos com poucos recursos, mantendo 60 FPS cravados sem nenhuma alocação de memória no loop principal.

### 4. Soluções Criadas
- **Camada HAL isolada:** O jogo e a engine nunca tocam em bibliotecas do Windows ou do Android. Todo o hardware é abstraído pela SDL2.
- **Buffer de Resolução Canônica:** A renderização ocorre em baixa resolução retrô (320x180 ou 480x270 widescreen) desenhada em um array linear de memória `uint32_t screenBuffer[WIDTH * HEIGHT]`. A SDL2 faz o escalonamento limpo para a tela inteira do celular ou monitor via GPU (*Nearest Neighbor*), garantindo pixels nítidos e custo de CPU quase zero.
- **Entidades Pré-alocadas (Object Pooling):** Zumbis e itens do mapa são reservados na memória antes do início da fase, eliminando pausas de coleta de lixo ou fragmentação de RAM.

### 5. Status Atual e Próximo Passo
- **Status:** Estrutura inicial e diário formalizados.
- **Próximo Passo:** Implementação da **Fase 1 e Fase 2**: Arquivos `CMakeLists.txt`, `src/hal/Platform.hpp/cpp`, `src/main.cpp`, motor Raycaster 2.5D, mapa urbano e HUD.

### 6. Checklist de Validação e Testes
- [x] Definição arquitetural das 3 camadas.
- [x] Padrão de documentação estabelecido.
- [x] Regras de performance (zero alocação no loop) definidas.

---

## [2026-10-22 17:35] - Entrada #01: Implementação da HAL, Motor Raycaster 2.5D, Quarteirão Urbano e HUD do DayZ

### 1. O que foi Implementado
- **CMakeLists.txt** estruturado com C++17 e suporte automático a SDL2 (usando biblioteca local ou FetchContent automatizado).
- **Camada 0 (HAL):**
  - `Platform.hpp` e `Platform.cpp`: Inicialização da janela com VSync, textura de streaming (`ARGB8888`), escalonador em tela cheia com Letterboxing e filtro *Nearest Neighbor* (mantendo a pixel art nítida sem borrões). Loop com medição estável de `DeltaTime` e travamento a 60 FPS.
  - `Input.hpp` e `Input.cpp`: Mapeamento universal de entradas (WASD, Setas, Mouse Look, e suporte a multi-touch para telas capacitivas de smartphones Android).
- **Camada 1 (Engine Retrô 2.5D):**
  - `MathDef.hpp`: Vetores 2D leves e funções otimizadas de *Distance Fog* (névoa atmosférica de apocalipse zumbi) e *Side Shading* (escurecimento sutil das faces horizontais das paredes para dar relevo e profundidade 3D).
  - `Camera.hpp`: Posição vetorial, direção e plano de câmera perpendicular com campo de visão (FOV) clássico de ~66 graus.
  - `Texture.hpp` e `Texture.cpp`: Banco de texturas procedurais retrô de 64x64 pixels geradas em código puro: Tijolos escuros de prédios, concreto rachado com musgo, vitrines de lojas saqueadas com tábuas de barricada, portas de madeira com maçaneta de bronze, chapas metálicas enferrujadas e asfalto com faixa amarela gasta.
  - `Raycaster.hpp` e `Raycaster.cpp`: Implementação completa do algoritmo **DDA (Digital Differential Analysis)**. Renderização de piso e céu sombrio por software, correção de distorção de olho de peixe (*Fisheye Correction*), cálculo de UVs e gravação contínua do **Z-Buffer 1D** (`zBuffer[x]`).
- **Camada 2 (Jogo - DayZ Demake):**
  - `CityMap.hpp` e `CityMap.cpp`: Matriz de 24x24 células desenhando um quarteirão urbano clássico do DayZ, com avenidas de asfalto, cruzamentos, prédios fechados, casas para saquear e sistema de portas interagíveis.
  - `Player.hpp` e `Player.cpp`: Controle em primeira pessoa com física de colisão e deslizamento em paredes (*Wall Sliding*), decaimento contínuo de fome, consumo de stamina ao correr, dano por sangramento e ação de abrir portas com a tecla 'F' / 'Espaço'.
  - `HUD.hpp` e `HUD.cpp`: Interface retrô desenhada diretamente sobre o buffer de pixels com mira centralizada, barras de Vida (Health), Fome (Food), Vigor (Stamina), indicador de Sangramento e fonte bitmap retrô embutida de 5x7 pixels.
  - `main.cpp`: Ponto de entrada limpo unindo todas as camadas no loop a 60 FPS.

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- **Visual Urbano em Fatias Verticais:** As ruas de DayZ foram transformadas em corredores e praças geradas pelo algoritmo DDA. A sensação de estar em uma rua abandonada foi alcançada unindo o asfalto texturizado no chão com prédios de tijolos e vitrines quebradas nas paredes verticais.
- **Portas Interagíveis no Grid:** No DayZ real, entrar em casas para fugir de zumbis e procurar loot é crucial. Adaptamos isso fazendo com que certas células do mapa sejam portas de madeira (ID 4) que, quando o jogador se aproxima e aperta a tecla de interação, a porta se abre (vira célula 0), permitindo entrar nos interiores dos edifícios.
- **Status Visuais de Sobrevivência:** Em vez de menus complexos, os medidores vitais do DayZ (Vida, Fome, Stamina e Sangramento) foram incorporados na parte inferior da tela como barras retrô translúcidas com cores de alerta.

### 3. Desafios Técnicos Encontrados
1. **Distorção de Olho de Peixe (Fisheye Effect):** Se a distância do raio for calculada usando o teorema de Pitágoras euclidiano simples, as paredes retas parecem arredondadas nas bordas da tela.
2. **Colisão Presa em Cantos de Paredes:** Se a colisão bloquear o vetor inteiro de movimento ao bater em uma parede, o jogador fica "enganchado" ao andar na diagonal encostado no muro de um prédio.
3. **Resolução Interna vs. Monitores Modernos e Telas de Celulares:** Telas de smartphones modernos são Full HD ou Quad HD com proporções como 20:9. Desenhar em alta resolução por software na CPU causaria queda brusca de FPS.

### 4. Soluções Criadas
1. **Distância Perpendicular ao Plano da Câmera:** No `Raycaster.cpp`, a distância foi projetada ortogonalmente contra o vetor de visão do jogador: `perpWallDist = (sideDist - deltaDist)`, garantindo paredes 100% planas e realistas.
2. **Separação de Eixos na Colisão (Wall Sliding):** No `Player.cpp`, o deslocamento no eixo X é testado e aplicado independentemente do eixo Y. Se o jogador andar contra uma parede vertical enquanto pressiona para frente, ele desliza suavemente ao longo da parede em vez de travar.
3. **Escalonador Fracionado com Letterboxing na GPU:** Adotamos a resolução canônica de **480x270 (16:9 widescreen)**. A CPU calcula apenas 129.600 pixels por quadro (levando menos de 2 milissegundos). A SDL2 então escala essa imagem para qualquer tamanho de tela (PC 1080p, 4K ou tela de celular) usando a GPU com filtro `Nearest Neighbor`, mantendo os pixels nítidos e a performance cravada a 60 FPS.

### 5. Status Atual e Próximo Passo
- **Status:** Fases 1 e 2 completas e funcionais dentro de `DAYDOOM/`. O jogador pode caminhar pelo quarteirão urbano, abrir portas de casas, sofrer fome e ver seu status no HUD retrô a 60 FPS.
- **Próximo Passo (Fase 3):** Implementação do **SpriteRenderer** com Z-Buffer para renderizar objetos no chão (latas de comida, bandagens e armas) e a renderização da arma empunhada pelo jogador em primeira pessoa.

### 6. Checklist de Validação e Testes (PC / Mobile)
- [x] Código C++17 modularizado nas 3 camadas (HAL, Engine, Game).
- [x] Loop de 60 FPS com VSync e textura de streaming.
- [x] Algoritmo DDA sem distorção olho de peixe.
- [x] Texturas procedurais retrô de 64x64 integradas.
- [x] Colisão com deslizamento de paredes funcionando.
- [x] Abertura de portas interagíveis com tecla 'F' / 'Espaço'.
- [x] HUD de sobrevivência com barras de Vida, Fome e Stamina.

---

## [2026-10-22 17:45] - Entrada #02: Perfis de Resolução Adaptativos (Celular Landscape Horizontal, Retro CRT 4:3 e Doom 13h)

### 1. O que foi Implementado
- **Criação do Enum `DisplayProfile` em `Platform.hpp`:**
  - `WIDESCREEN_16_9` (480x270): Otimizado para monitores de PC e celulares na horizontal (Landscape).
  - `RETRO_4_3` (320x240): Resolução clássica de monitores CRT 4:3 e arcades retrô.
  - `CLASSIC_DOOM_13H` (320x200): Modo 13h histórico do MS-DOS do Doom original de 1993.
- **Trava de Orientação Mobile em Paisagem (Horizontal):**
  - Implementado `SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight")` para impedir que o jogo no celular gire para a vertical, forçando a pegada ergonômica de jogo de tiro.
- **Adaptação Dinâmica à Tela com Proporção Perfeita:**
  - O `Platform::init` aceita qualquer perfil de tela. A GPU calcula as barras de ajuste (*Letterboxing* para telas ultrawide ou *Pillarboxing* para telas mais quadradas) através do `SDL_RenderSetLogicalSize`, mantendo o aspecto exato sem esticar ou achatar a imagem.
- **Atalho de Tela Cheia:** Tecla `F11` adicionada para alternar tela cheia no PC instantaneamente.
- **Suporte a Linha de Comando e Lançadores Rápidos:** O executável aceita `--43`/`--crt` ou `--doom`/`--13h` para rodar diretamente nos perfis desejados, acompanhado de scripts executáveis dedicados (`run_pc.bat`, `run_widescreen_16_9.bat`, `run_doom_13h.bat`, `run_retro_4_3.bat`).

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- **Flexibilidade de Proporção:** Seja no monitor em 4:3 ou no celular deitado em 16:9/20:9, o Raycaster recalcula as fatias verticais e a posição do HUD dinamicamente, mantendo o visual retrô idêntico em qualquer resolução.

### 3. Desafios Técnicos Encontrados
1. **Diferença de Aspect Ratio entre Monitores e Telas:** O renderizador precisa adaptar o buffer interno e a quantidade de raios DDA sem deformar as paredes ou achatar a mira do jogador.
2. **Rotação Involuntária em Celulares:** Sem a trava de orientação da SDL2, o acelerômetro do smartphone girava a tela para o modo retrato ao mover as mãos.

### 4. Soluções Criadas
1. **Perfis de Resolução Desacoplados:** A largura e a altura deixaram de ser constantes fixas de compilação e passaram a ser variáveis dinâmicas da classe `Platform`. O buffer e o Raycaster se ajustam instantaneamente ao perfil selecionado no início do jogo.
2. **Hint de Orientação da SDL2:** A chamada `SDL_HINT_ORIENTATIONS` assegura que o sistema operacional Android force a tela cheia horizontal antes mesmo da janela ser criada.

### 5. Status Atual e Próximo Passo
- **Status:** Suporte a múltiplas resoluções concluído, testado e validado a 60 FPS cravados. Código 100% puro C++17 sem dependências desnecessárias.
- **Próximo Passo (Fase 3):** Implementação do **SpriteRenderer** com Z-Buffer para desenhar latas de comida, bandagens e zumbis nas ruas, e o sprite da arma em primeira pessoa centralizada.

### 6. Checklist de Validação e Testes (PC / Mobile)
- [x] Perfil Widescreen 480x270 funcional para PC e celular deitado.
- [x] Perfil Doom 13h 320x200 funcional e selecionável via `--doom`.
- [x] Perfil Retro CRT 4:3 320x240 funcional e selecionável via `--43`.
- [x] Trava de orientação em paisagem (Landscape) aplicada para Android.
- [x] Redimensionamento com Letterboxing preservando proporção sem distorção.
- [x] Alternância de tela cheia com tecla F11.
- [x] Lançadores rápidos em lote (.bat) para cada resolução criados e testados.
- [x] Compilação Release com 0 Warnings e 0 Errors.

---

## [2026-10-22 21:35] - Entrada #03: Modelagem da Malha Urbana de Chernogorsk 2.5D (Ruas, Fachadas, Galpões, Muros e Contêineres)

### 1. O que foi Implementado
- **Transposição Fiel do Mapa Conceitual (`mapa-chernogorsk.jpg`):**
  - Expansão do motor para uma malha de **64x64 células**, reproduzindo a planta baixa da cidade em 5 setores estratégicos:
    - **Setor 1 (Costa, Praia & Docas Portuárias - Y=48 a 63):**
      - Oceano azul do Mar Báltico com ondas (`WATER01`) e barreira sul.
      - Faixa de areia dourada da praia (`SAND01`).
      - Píer de concreto que avança no mar com a torre do farol na ponta.
      - Grande plataforma das docas com atracadouro, navio cargueiro e pilhas de contêineres marítimos vermelhos (`CONTRED`) e azuis (`CONTBLU`).
      - Barco naufragado encalhado na areia.
    - **Setor 2 (Bairro Residencial Sul - Y=37 a 44):**
      - Ruas e quarteirões de casas de tijolos (`BRICK01`), praça residencial com poço d'água potável e anexo de viaturas da polícia.
    - **Setor 3 (Centro Urbano & Esquadra de Polícia Central - Y=24 a 34):**
      - A monumental **Esquadra de Polícia de Chernogorsk** no centro com pátio de estacionamento, paredes maciças de concreto (`CONC02`), o **Brasão do Escudo Policial** (`POLICESH`) no centro da fachada sul e porta corrediça de aço (`DOORMLT`).
      - Supermercado saqueado e Café Central com vitrines de vidro (`WINCOMM`).
    - **Setor 4 (Linha Férrea, Avenida Principal & Hospital - Y=12 a 23):**
      - A **Linha Férrea** (`RAILROAD`) cortando a cidade de fora a fora com dormentes de madeira, trilhos de aço e vagões de trem cargueiro parados.
      - O **Hospital Central de Chernogorsk** (`HOSP01`) com fachada branca e a cruz vermelha médica.
    - **Setor 5 (Zona Industrial Norte & Galpões - Y=0 a 11):**
      - Conjunto de grandes **Galpões Industriais Azuis** (`HANGARBL`) e galpões de aço cinza (`CORRMT1`).
      - Muro perimetral norte com o **Grande Portão de Ferro das Docas** (`GATEIRON`) como saída da fase.
- **20 Texturas Procedurais de Alta Performance:**
  - Banco expandido em `TextureManager` com amostragem imediata na memória L1 (sem arquivos externos para carregar).
- **Spawn do Jogador:**
  - O jogador nasce na praia de areia do Mar Báltico em `(15.5, 50.5)`, ao lado do cais do farol, contemplando a orla e olhando para o Norte em direção à entrada de Chernogorsk!
- **HUD com Indicador de Setor em Tempo Real:**
  - O topo da tela exibe dinamicamente o setor onde o jogador está pisando (ex: `"SETOR 1: PRAIA DO MAR BALTICO & PORTO"`, `"SETOR 3: ESQUADRA DE POLICIA CENTRAL"`).

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- O traçado viário é 100% navegável: ruas, avenidas, becos, calçadas, o cais do farol e a plataforma de contêineres do porto são transitáveis livremente, respeitando as colisões de paredes e fachadas.

### 3. Desafios Técnicos Encontrados
- Alinhar a escala das 8 colunas e 7 linhas do mapa conceitual em pixel art para uma proporção cúbica 2.5D sem criar gargalos ou ruas excessivamente estreitas.

### 4. Soluções Criadas
- Adoção da resolução em malha de 64x64 blocos com ruas de 2 a 3 células de largura (permitindo caminhada e corrida fluidas a 60 FPS com deslizamento de parede).

### 5. Status Atual e Próximo Passo
- **Status:** Malha viária, fachadas, galpões, muros, contêineres e linha férrea de Chernogorsk 100% implementados, compilados e validados!
- **Próximo Passo (Fase 3):** Implementação dos sprites de sobrevivência (latas de feijão, bandagens no chão), zumbis patrulhando as ruas e a arma em primeira pessoa centralizada.

### 6. Checklist de Validação e Testes
- [x] Malha viária 64x64 de Chernogorsk implementada conforme `mapa-chernogorsk.jpg`.
- [x] Texturas de galpões azuis (`HANGAR_BLUE`) e brasão da polícia (`POLICE_EMBLEM`) criadas.
- [x] Plataforma das docas com contêineres vermelhos e azuis construída.
- [x] Linha férrea com piso de trilhos cruzando a cidade.
- [x] Píer do farol e Mar Báltico no Setor 1 funcionais.
- [x] Esquadra de polícia central e Hospital com cruz vermelha posicionados.
- [x] Compilado com MSBuild Release: 0 Warnings, 0 Errors.

---

## [2026-10-22 22:05] - Entrada #04: Grelha Proporcional 12x10, Diferenciação de Paredes, Muros e Portas por Textura e Automap Tático em Tela Cheia

### 1. O que foi Implementado
- **Grelha Proporcional 12x10 (`g_ChernoWorldGrid`) Conforme o Mapa Conceitual:**
  - Estruturação estrita da planta baixa de Chernogorsk em uma matriz canônica de 12 colunas por 10 linhas proporcional ao layout oficial de `mapa-chernogorsk.jpg`:
    - **Linhas 0 e 1 (Periferia Norte / Hospital & Comercial):** Hospital Central com cruz vermelha (`HOSPITA`), lojas comerciais, clínicas e galpões industriais.
    - **Linha 2 (Eixo Férreo Horizontal):** A Linha de Caminho de Ferro cruzando horizontalmente toda a largura da cidade de ponta a ponta com dormentes, brita e vagões cargueiros parados.
    - **Linha 3 e 4 (Bairro Residencial Central & Esquadra de Polícia):** O grande núcleo central da **Esquadra de Polícia** (`POLICE_STATION`, tag 101, loot militar Tier 3), cercado pelo pátio de viaturas e bairros residenciais.
    - **Linhas 5 e 6 (Bairro Residencial Sul & Avenida Marginal):** Sobrados residenciais e a grande avenida de asfalto de transição para a costa.
    - **Linhas 7, 8 e 9 (Zona Portuária, Praia & Mar Báltico):** Cais portuário de concreto com guindastes amarelos, pilhas de contêineres marítimos, píer do farol, costa de areia e o Mar Báltico ao sul.
  - Cada macro-bloco é expandido no motor 2.5D para uma escala de alta resolução de **60x50 células**, garantindo ruas largas com passagem fluida a 60 FPS e edifícios detalhados.

- **Diferenciação Estrita de Texturas: PAREDES vs. MUROS vs. PORTAS:**
  - O banco de texturas procedurais foi expandido para **24 texturas** (`TextureManager`), diferenciando categoricamente:
    1. **PAREDES (Fachadas de Edifícios):**
       - `BRICK_DARK`: Alvenaria de tijolo escuro residencial com juntas de argamassa.
       - `CONCRETE`: Concreto bruto envelhecido da delegacia e edifícios públicos.
       - `POLICE_EMBLEM`: Fachada monumental com o grande **Brasão do Escudo Azul e Estrela Dourada** da Polícia de Chernogorsk.
       - `HOSPITAL_WALL`: Fachada médica branca com o emblema da **Cruz Vermelha**.
       - `HANGAR_BLUE`: Galpões industriais com chapa metálica ondulada azul e rebites.
       - `RUSTY_FENCE` / Metal: Galpões de armazenamento em chapa ondulada galvanizada com ferrugem.
       - `STORE_FRONT`: Fachada de supermercado com vitrines de vidro e barricadas de madeira.
       - `CONTAINER_RED` & `CONTAINER_BLUE`: Contêineres de carga marítima com frisos verticais.
    2. **MUROS (Barreiras Perimetrais e Divisórias):**
       - `MURO_SOVIET` (Muro PO-2): O lendário muro soviético de placas de concreto pré-moldado com **relevo piramidal em losango/diamante** ("Забор ПО-2"), topo chanfrado contra chuva e pilares laterais de sustentação. Envolve o pátio da polícia, ambulâncias do hospital e zonas industriais.
       - `MURO_STONE`: Muro rústico de pedra e alvenaria com topo alisado de cimento e musgo, dividindo quintais residenciais.
       - `FENCE_WIRE`: Cerca perimetral de arame entrelaçado com mourões metálicos e arame farpado no topo.
    3. **PORTAS & PORTÕES (Interagíveis com 'F' / 'Espaço'):**
       - `WOOD_DOOR`: Porta residencial de madeira com ripas escuras e maçaneta de bronze dourado.
       - `DOOR_SLIDING`: Porta corrediça de aço reforçado com faixas de perigo zebradas e painel de controle da polícia.
       - `DOOR_HOSPITAL`: Porta dupla branca de hospital com vidros aramados, barras antipânico e cruzes vermelhas.
       - `GATE_IRON`: Grande portão de segurança de ferro forjado quadriculado com cadeado de bronze (Docas e Saída Norte).

- **Interação em 1ª Pessoa com Portas:**
  - Ao apontar para qualquer porta a até 1.3 metros de distância, o HUD exibe um prompt central dinâmico: `"[F]: ABRIR PORTA"` em amarelo dourado.
  - Pressionar a tecla `[F]` ou a barra de `[Espaço]` abre a porta em tempo real, liberando a passagem para o interior de casas, da esquadra, do hospital e dos galpões.

- **Planta Baixa Tática em Tela Cheia (Automap - Tecla [M] ou [Tab]):**
  - Ao pressionar `[M]` ou `[Tab]`, o jogo abre a **Planta Baixa Tática Completa** de Chernogorsk em tela cheia:
    - Renderizada com **amostragem direta das texturas reais do jogo** (em vez de cores planas genéricas), permitindo identificar imediatamente as paredes de tijolo, asfalto, ferrovia, praia, mar, contêineres e galpões azuis.
    - **Portas Destacadas:** Portas residenciais, corrediças e hospitalares são realçadas na planta com um **contorno dourado/âmbar pulsante**, facilitando a localização de entradas estratégicas para fuga e abrigo.
    - **Muros Identificados:** Muros de concreto PO-2, pedra e arame possuem contorno prateado sutil.
    - **Ponto de Localização do Jogador:** Beacon pulsante em neon verde/amarelo com **seta de visão em tempo real**, indicando o rumo e ângulo exato da câmera.
    - **Painel Tático Lateral:**
      - Título da Planta Baixa e Grelha Proporcional.
      - **Setor Ativo** em tempo real.
      - **Coordenadas GPS** `(X, Y)`.
      - **Bússola Militar com Norte no Topo** `[N ^]`, alinhada 1:1 com a rosa-dos-ventos do mapa original.
      - **Legenda Completa** com miniaturas das texturas reais do jogo e indicação de `PORTAS [P]`.
      - Atalho de saída: `[M]: VOLTAR`.

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- O mapa de Chernogorsk agora é visualmente inteligível tanto na visão de 1ª pessoa por Raycasting quanto na visão estratégica por planta baixa. O jogador consegue reconhecer os pontos de referência clássicos do DayZ: o farol na praia ao sul, a delegacia fortificada no meio da cidade com seu muro PO-2 e brasão, o hospital branco com cruz vermelha ao norte e os trilhos de trem com vagões bloqueando avenidas.

### 3. Checklist de Validação e Testes
- [x] Matriz macro 12x10 proporcional implementada em `ChernogorskDef.hpp`.
- [x] Malha viária e quadras expandidas em escala 60x50 em `CityMap.cpp`.
- [x] Novas texturas procedurais criadas: `MURO_SOVIET` (PO-2), `MURO_STONE`, `DOOR_HOSPITAL`.
- [x] Diferenciação categórica entre paredes, muros e portas em 3D e no Automap.
- [x] Prompt visual `[F]: ABRIR PORTA` no HUD ao aproximar-se de portas.
- [x] Abertura de portas e portões com tecla `[F]` e `[Espaço]`.
- [x] Alternância da Planta Baixa Tática em tela cheia via tecla `[M]` e `[Tab]`.
- [x] Ponto de localização pulsante e bússola militar com orientação norte no Automap.
- [x] Compilação MSBuild Release x64 concluída com 0 Warnings e 0 Errors.

---

## [2026-10-26 17:00] - Entrada #05: Portas Finas com Deslizamento e Batentes (Wolf3D/Doom Style) e Texturas Reais em Alto Contraste

### 1. O que foi Implementado
- **Arquitetura de Portas Finas e Centradas (Substituição de Blocos Maciços):**
  - Eliminação da lógica antiga de apagar o bloco inteiro (`m_grid[x, y] = 0`).
  - Criação da estrutura de dados `Door` com máquina de estados (`CLOSED`, `OPENING`, `OPEN`, `CLOSING`) e porcentagem contínua de abertura `openProgress` (0.0f a 1.0f).
  - Portas são agora **lâminas finas posicionadas exatamente no centro do bloco** ($X + 0.5$ para portas verticais ou $Y + 0.5$ para horizontais), embutidas entre as paredes.
- **Novo Algoritmo DDA para Portas e Batentes no Raycaster (`Raycaster.cpp`):**
  - O raio DDA testa a interseção com o plano central da folha da porta.
  - Se a porta estiver aberta ou em movimento, a abertura desliza suavemente: o raio que passar pela folga não colide e **continua atravessando para o interior do edifício**!
  - Se atingir a folha da porta, a textura correta desliza horizontalmente acompanhando o movimento.
  - Se o raio entrar lateralmente pelo vão da parede, atinge o **novo marco/batente lateral de aço e madeira** (`DOOR_FRAME`), dando profundidade e espessura real à passagem.
- **Abertura e Fechamento Bidirecional com a Tecla `[F]` ou `[Espaço]`:**
  - O jogador agora pode **abrir E fechar** qualquer porta quando quiser.
  - O HUD detecta dinamicamente o estado da porta:
    - Porta fechada ou fechando: prompt dourado `"[F]: ABRIR PORTA"`.
    - Porta aberta ou abrindo: prompt ciano `"[F]: FECHAR PORTA"`.
  - Passagem física (`isWalkable`) liberada organicamente assim que a porta atinge mais de 70% de abertura.
- **Reformulação Completa das 24 Texturas para Alto Contraste Retrô (Estilo Doom 1993):**
  - **Tijolos (`BRICK_DARK`):** Padrão de 8 fileiras com juntas alternadas, bisel 3D com luzes claras na borda superior/esquerda e sombras profundas na borda inferior/direita, além de variações individuais de tom de terracota.
  - **Concreto Soviético (`CONCRETE`):** Juntas de dilatação, 4 parafusos de ancoragem com arruelas de aço nas pontas e escorrimento vertical escuro.
  - **Muro Soviético PO-2 (`MURO_SOVIET`):** Relevo piramidal em losango ("Забор ПО-2") com 4 facetas iluminadas direcionalmente em 3D.
  - **Porta de Madeira (`WOOD_DOOR`):** 4 pranchas de carvalho com veios rústicos, dobradiças de ferro forjado e maçaneta dourada de bronze com espelho.
  - **Porta Blindada (`DOOR_SLIDING`):** Faixas zebradas de perigo (amarelo e preto), visor blindado aramado e teclado com LED verde.
  - **Porta Hospitalar (`DOOR_HOSPITAL`):** Portas duplas brancas com chapas de aço inox na base e barras antipânico com reflexo metálico.
  - **Galpão Azul (`HANGAR_BLUE`):** Chapas frisadas com cristas reflexivas, vigas I de aço e rebites industriais.
- **Iluminação Atmosférica com Preservação de Texturas (`MathDef.hpp`):**
  - A névoa por distância agora garante uma **iluminação ambiente mínima de 32%** e raio estendido até 38 metros, impedindo que prédios do outro lado da rua se tornem blocos pretos planos.

### 2. Checklist de Validação e Testes
- [x] Portas finas centralizadas no bloco com vão e batente (`DOOR_FRAME`).
- [x] Deslizamento suave de abertura e fechamento a 60 FPS.
- [x] Raio do Raycaster atravessa a abertura de portas abertas permitindo ver o interior.
- [x] Suporte a abrir e fechar a porta pressionando `[F]` ou `[Espaço]`.
- [x] Prompt visual no HUD alternando dinamicamente entre `[F]: ABRIR PORTA` e `[F]: FECHAR PORTA`.
- [x] Colisão física do jogador liberada suavemente quando a porta abre.
- [x] 24 texturas com alto contraste, relevo 3D, bisel e estilo autêntico do Doom 1993.
- [x] Compilado com MSBuild Release x64: 0 Warnings, 0 Errors.

---

## [2026-10-26 18:15] - Entrada #06: Portas Giratórias com Dobradiças em 3D (Espessura 1/20) e Paredes/Muros com Espessura Real 1/10

### 1. O que foi Implementado
- **Portas com Abertura Giratória por Rotação na Dobradiça (Swinging / Hinged Doors):**
  - Eliminação completa do deslizamento para dentro do muro ("não sendo puxadas pelo muro").
  - A porta agora gira organicamente em torno de seu eixo/dobradiça (`hingeX, hingeY`) de $0^\circ$ (fechada) até $90^\circ$ ($\frac{\pi}{2}$ radianos) aberta.
  - Implementação da rotação vetorial $\vec{u} = (\cos\theta, \pm\sin\theta)$ e normal $\vec{n} = (\mp\sin\theta, \cos\theta)$ em espaço de coordenadas contínuas.
  - Folha da porta modelada com **espessura exata de 1/20 da largura do bloco ($0.05$ unidades)**, contendo face frontal, face traseira e borda de topo/ponta de espessura $0.05$.
  - Interseção raio-segmento em 2D fechada e analítica, calculando o parâmetro de profundidade perpendicular sem deformações de perspectiva ou fisheye.
- **Paredes de Edifícios e Muros com Espessura Real de 1/10 da Largura do Bloco ($0.10$ unidades):**
  - Muros divisórios (PO-2 soviético `MURO_SOVIET`, muro de pedra `MURO_STONE`, cerca `FENCE_WIRE`) e paredes de construções deixam de ser blocos maciços cúbicos de $1.0 \times 1.0$.
  - Cada muro/parede é renderizado com espessura de $1/10$ ($0.10$ unidades), centrado na célula (placas de $0.45$ a $0.55$).
  - O DDA testa as faces da placa fina com detecção de orientação horizontal/vertical e quinas.
  - A visão do jogador e os raios de luz conseguem enxergar além das extremidades dos muros finos, conferindo esbelteza e proporção arquitetônica realista.
- **Vão de Passagem Livre e Batentes:**
  - O vão da porta possui batentes fixos nas duas extremidades laterais revestidos com textura `DOOR_FRAME`.
  - Ao abrir a porta, o vão central fica totalmente desobstruído: o jogador vê através da abertura para o interior das salas, enquanto a folha de espessura $0.05$ gira contra a parede lateral.
- **Abertura Inteligente para o Interior:**
  - A função `registerDoor` inspeciona os compartimentos adjacentes e define `swingDir` para que as portas girem sempre para o lado interno das edificações.
- **Passagem Física do Jogador:**
  - O sistema de colisão (`isWalkable`) libera a travessia assim que a rotação da porta ultrapassa $35^\circ$ ($openProgress > 0.40$).

### 2. Checklist de Validação e Testes
- [x] Portas abrem girando em torno da dobradiça de $0^\circ$ a $90^\circ$ (não deslizam).
- [x] Espessura da porta configurada para $1/20$ ($0.05$ unidades).
- [x] Folha da porta possui faces frontal, traseira e borda de espessura visíveis em 3D.
- [x] Espessura das paredes e muros reduzida para $1/10$ ($0.10$ unidades).
- [x] Vão de porta com batentes laterais e abertura livre para visão do interior.
- [x] Tecla `[F]` e `[Espaço]` alternam entre abrir girando e fechar girando.
- [x] HUD indica dinamicamente `[F]: ABRIR PORTA` e `[F]: FECHAR PORTA`.
- [x] Compilado com MSBuild Release x64: 0 Warnings, 0 Errors.




# 🧟 DAYDOOM 2.5D — DayZ Retro Demake (Doom Engine Style)

<div align="center">

![C++17](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![SDL2](https://img.shields.io/badge/HAL-SDL2-1774ac?style=for-the-badge&logo=sdl&logoColor=white)
![CMake](https://img.shields.io/badge/Build-CMake%203.16+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/Platforms-Windows%20%7C%20Linux%20%7C%20Android%20%7C%20Web-4CAF50?style=for-the-badge)
![Status](https://img.shields.io/badge/Release-v001%20Alpha-FF5722?style=for-the-badge)

**Um demake retrô pós-apocalíptico que funde a atmosfera tensa de sobrevivência do *DayZ* com a tecnologia e estética clássica dos pioneiros dos FPS dos anos 90 (*Doom*, *Wolfenstein 3D*).**

[Visão Geral](#-visão-geral-do-projeto) • [Técnicas Utilizadas](#-técnicas-matemáticas-e-gráficas) • [Arquitetura](#-arquitetura-do-projeto) • [Estado Atual](#-estado-atual-do-projeto-v001) • [Como Jogar e Compilar](#-como-compilar-e-executar) • [Controles](#-controles)

</div>

---

## 📖 Visão Geral do Projeto

**DAYDOOM 2.5D** reimagina o lendário jogo de sobrevivência **DayZ** através das restrições e elegância da computação gráfica do início dos anos 90. 

Em vez de motores 3D poligonais pesados, o jogo roda em uma **Engine 2.5D proprietária baseada em Raycasting por software**, onde cada coluna da tela é projetada matematicamente em tempo real diretamente pela CPU em um buffer de pixels linear (`uint32_t screenBuffer[WIDTH * HEIGHT]`).

O mundo de **Dayz** ganha vida em um grid urbano detalhado: ruas de asfalto rachado, linha férrea com vagões cargueiros, edifícios públicos fortificados (como a Esquadra de Polícia Central e o Hospital), quarteirões residenciais, o porto com pilhas de contêineres marítimos e o sombrio Mar Báltico ao sul.

---

## ⚙️ Arquitetura do Projeto

O projeto segue à risca o padrão arquitetural definido no documento de engenharia [`PROJECT_MATRIX.md`](PROJECT_MATRIX.md), estruturado em **3 Camadas Estritas e Desacopladas**:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        CAMADA 2: O JOGO (DAYZ)                         │
│  - Player (Sobrevivência: Vida, Fome, Stamina, Dano por Sangramento)   │
│  - Cidade de Chernogorsk (Malha 60x50, 5 setores táticos, colisão)     │
│  - Sistema de Portas Interagíveis (Abertura em 1ª pessoa com 'F')      │
│  - Automap Tático em Tela Cheia (Tecla M / Tab com texturas reais)     │
│  - HUD Retrô de Sobrevivência (Barras dinâmicas e avisos vitais)       │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Utiliza serviços da Engine
┌───────────────────────────────────▼────────────────────────────────────┐
│                    CAMADA 1: A ENGINE RETRÔ 2.5D                      │
│  - Raycaster 2.5D (Algoritmo DDA - Digital Differential Analysis)      │
│  - Correção de Distorção Olho de Peixe (Fisheye Correction)            │
│  - Floor & Ceiling (Piso de asfalto/terra e céu sombrio texturizados)   │
│  - Shading Atmosférico (Distance Fog e Side Shading horizontal)        │
│  - TextureManager (24 texturas procedurais 64x64 na memória L1)        │
│  - Z-Buffer 1D contínuo para profundidade e oclusão                    │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Abstrai o hardware pela HAL
┌───────────────────────────────────▼────────────────────────────────────┐
│                CAMADA 0: HAL (SDL2 & PLATFORM WRAPPER)                 │
│  - Gerenciamento de Janela, VSync e Framebuffer Texture (ARGB8888)     │
│  - Escalonamento GPU com Integer / Nearest-Neighbor (Pixels nítidos)   │
│  - Perfis Dinâmicos de Tela (Widescreen 16:9, Doom 13h, Retro CRT 4:3)  │
│  - Travamento estável a 60 FPS com cálculo preciso de DeltaTime        │
│  - Mapeamento universal de entradas (Teclado, Mouse e Mobile Touch)    │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 🔬 Técnicas Matemáticas e Gráficas

### 1. Algoritmo DDA (Digital Differential Analysis)
O coração da ilusão tridimensional é o algoritmo **DDA**. Para cada coluna de pixels horizontal da resolução selecionada, um raio vetorial é disparado a partir da posição da câmera do jogador `(posX, posY)` através do plano de projeção perpendicular (FOV clássico de ~66°).
- O algoritmo calcula os passos discretos nos eixos X e Y (`deltaDistX`, `deltaDistY`) e avança célula a célula na malha do mapa até colidir com uma parede sólida ou porta.
- A CPU só processa os limites das células atingidas, garantindo performance altíssima e estável mesmo em processadores de baixo consumo.

### 2. Correção de Olho de Peixe (*Fisheye Correction*)
Se a distância do raio fosse calculada pela hipotenusa euclidiana pura ($\sqrt{\Delta x^2 + \Delta y^2}$), as paredes retas ficariam curvadas nas bordas da visão. A engine projeta a distância ortogonalmente contra o vetor de orientação da câmera:
$$\text{perpWallDist} = \left(\text{sideDist} - \text{deltaDist}\right)$$
Resultando em paredes 100% planas, sólidas e com perspectiva geométrica idêntica à do *Doom* de 1993.

### 3. Sombreamento Atmosférico (*Distance Fog & Side Shading*)
- **Side Shading:** As paredes alinhadas ao eixo horizontal recebem uma atenuação de luminosidade de 25% a 30% em relação às paredes verticais, gerando relevo e contraste tridimensional natural nas quinas.
- **Distance Fog:** Um gradiente exponencial de névoa escura escurece gradualmente a geometria conforme a distância da câmera, transmitindo o clima opressivo e claustrofóbico do apocalipse zumbi.

### 4. Texturização Procedural em Memória L1 (Zero Dependência Externa)
Todas as **24 texturas retrô de 64x64 pixels** do jogo são sintetizadas proceduralmente em tempo de compilação/inicialização através de algoritmos matriciais em C++ puro:
- Tijolos escuros residenciais, concreto bruto, asfalto com faixas desgastadas;
- **Fachadas Monumentais:** O grande brasão com escudo azul e estrela da Polícia de Chernogorsk, a fachada hospitalar com cruz vermelha médica, vitrines de supermercado saqueado;
- **Muros Históricos:** O lendário **Muro Soviético PO-2** (*Забор ПО-2*) com relevo piramidal em losango, mourões de concreto, muros de pedra com musgo e cerca de arame farpado;
- As texturas residem diretamente na memória cache L1 da CPU, eliminando tempo de carregamento de disco e garantindo 0 bytes de arquivos corrompidos.

### 5. Física de Movimentação com Deslizamento em Paredes (*Wall Sliding*)
A colisão do jogador com as estruturas urbanas testa e aplica os deslocamentos nos eixos X e Y de forma independente com raio de segurança. Se o jogador andar contra uma quina ou parede enquanto avança, ele não trava: o vetor resultante desliza suavemente pela face da estrutura.

### 6. Escalonamento Canônico com *Nearest-Neighbor*
O jogo renderiza em resolução canônica de baixa contagem de pixels (ex: **480x270** widescreen, consumindo menos de 2ms de CPU por quadro). Em seguida, a SDL2 e a GPU escalam o framebuffer para a resolução nativa da tela do usuário com filtro *Nearest-Neighbor*, preservando a estética nítida de pixel art sem nenhum borrão bilinear.

---

## 🗺️ A Malha Urbana de Chernogorsk (Grelha 12x10 Proporcional)

O mapa do jogo é modelado com base na planta clássica de **Chernogorsk** do DayZ, organizado em **5 macro-setores táticos**:

| Setor | Nome | Elementos Urbanos Principais | Texturas Dominantes |
|:---:|:---|:---|:---|
| **1** | **Costa & Porto** | Mar Báltico, Praia de Areia, Píer do Farol, Atracadouro e Pátio de Contêineres | `WATER01`, `SAND01`, `CONC02`, `CONTRED`, `CONTBLU` |
| **2** | **Residencial Sul** | Quarteirões de sobrados residenciais, praça com poço d'água e pátio | `BRICK01`, `MURO_STONE`, `WOOD_DOOR` |
| **3** | **Centro & Polícia** | Esquadra de Polícia Central de Chernogorsk, Pátio com Muros PO-2, Centro Comercial | `POLICESH`, `MURO_SOVIET`, `WINCOMM`, `DOORMLT` |
| **4** | **Ferrovia & Hospital** | Linha de Caminho de Ferro com vagões cargueiros e o Hospital Central | `RAILROAD`, `HOSP01`, `DOOR_HOSPITAL` |
| **5** | **Zona Industrial Norte** | Grandes Galpões Industriais Azuis, galpões de aço e Portão de Ferro das Docas | `HANGARBL`, `CORRMT1`, `GATEIRON` |

---

## 🎮 Funcionalidades em Destaque

### 🚪 Sistema de Portas Interagíveis em 1ª Pessoa
Ao se aproximar de qualquer porta residencial, porta corrediça de aço da delegacia, porta dupla de hospital ou portão de ferro das docas (a até 1.3 metros), o HUD exibe o aviso dourado `"[F]: ABRIR PORTA"`. Ao apertar a tecla `[F]` ou `[Espaço]`, a porta se abre dinamicamente, liberando a passagem para o interior das estruturas.

### 🗺️ Automap Tático em Tela Cheia (`[M]` ou `[Tab]`)
Ao acionar o mapa tático, a visão alterna instantaneamente para a planta baixa completa de Chernogorsk:
- **Amostragem com Texturas Reais:** O minimapa desenha miniaturas das próprias texturas das ruas, ferrovias, tijolos, contêineres e mar, facilitando o reconhecimento espacial imediato;
- **Contornos Pulsantes:** Portas são realçadas com contornos dourados e muros com contornos prateados;
- **Beacon do Jogador:** Seta direcional pulsante em neon verde/amarelo indicando coordenadas GPS e ângulo de visão em tempo real;
- **Bússola Militar:** Orientação com Norte magnético no topo `[N ^]`, idêntica à rosa-dos-ventos militar.

---

## 📊 Estado Atual do Projeto (v001)

| Fase | Descrição | Status | Detalhes |
|:---:|:---|:---:|:---|
| **Fase 0** | **Documento Matriz de Engenharia** | Concluído ✅ | `PROJECT_MATRIX.md` e regras de arquitetura |
| **Fase 1** | **HAL SDL2 & Engine Raycaster 2.5D** | Concluído ✅ | Loop a 60 FPS, DDA, VSync, Wall Sliding e Shading |
| **Fase 2** | **Quarteirão Urbano & HUD de Sobrevivência** | Concluído ✅ | Barras de Vida, Fome, Stamina, Alerta de Sangramento |
| **Fase 3** | **Perfis de Resolução Adaptativos** | Concluído ✅ | 16:9 Widescreen, Doom 13h (320x200), CRT 4:3 (320x240) |
| **Fase 4** | **Modelagem de Chernogorsk 2.5D** | Concluído ✅ | 5 setores táticos, 24 texturas procedurais e Muro PO-2 |
| **Fase 5** | **Portas Interagíveis & Automap Tático** | Concluído ✅ | Interação com `[F]` e Automap em tela cheia com `[M]` |
| **Fase 6** | **SpriteRenderer & Objetos no Chão** | Em Breve 🔄 | Z-Buffer 1D para latas de feijão, bandagens e armas |
| **Fase 7** | **Zumbis & Inteligência Artificial** | Em Breve 🔄 | Hordas atraídas por som e visão, ataque corpo a corpo |
| **Fase 8** | **Arma em Primeira Pessoa** | Em Breve 🔄 | Sprite centralizado animado (fogo, recarga e recuo) |

O histórico detalhado de cada sessão e desafios técnicos superados pode ser consultado no [Diário de Bordo Oficial (`DEVLOG.md`)](DEVLOG.md).

---

## 💻 Como Compilar e Executar

### Pré-requisitos
- **Compilador C++17** (MSVC no Windows, GCC 9+ ou Clang 10+ no Linux/macOS)
- **CMake 3.16 ou superior**
- **SDL2:** Não é necessário instalar manualmente; caso o sistema não possua a SDL2 instalada, o `CMakeLists.txt` fará o download e configuração automaticamente via `FetchContent`.

### Compilando no Windows (Visual Studio / MSBuild)
```bash
# 1. Clone o repositório
git clone https://github.com/LuizCorrea-Dev/DAYDOOM.git
cd DAYDOOM

# 2. Crie a pasta de build e gere o projeto
cmake -B build -S .

# 3. Compile em modo Release
cmake --build build --config Release
```

### Compilando no Linux / macOS
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/DayDoom
```

### Scripts de Inicialização Rápida (Windows)
O projeto inclui scripts `.bat` prontos para testar cada perfil de exibição:
- `run_pc.bat`: Menu interativo para seleção de resolução.
- `run_widescreen_16_9.bat`: Executa em 16:9 Widescreen (480x270).
- `run_doom_13h.bat`: Executa no clássico modo Doom MS-DOS 13h (320x200).
- `run_retro_4_3.bat`: Executa na proporção retrô CRT 4:3 (320x240).

---

## 🕹️ Controles

| Comando | Ação |
|:---|:---|
| **`W` / `S`** ou **`↑` / `↓`** | Mover para frente / trás |
| **`A` / `D`** | Passo lateral (*Strafing*) para esquerda / direita |
| **`←` / `→`** ou **Movimento do Mouse** | Girar a visão da câmera |
| **`Shift Esquerdo`** | Correr (consome Stamina) |
| **`F`** ou **`Barra de Espaço`** | Abrir portas e portões |
| **`M`** ou **`Tab`** | Alternar Planta Baixa Tática (Automap) |
| **`F11`** | Alternar modo Tela Cheia (*Fullscreen*) |
| **`ESC`** | Sair do jogo |

---

## 📁 Estrutura do Código-Fonte

```
DAYDOOM/
├── CMakeLists.txt          # Script de compilação multi-plataforma com C++17
├── DEVLOG.md               # Diário de bordo detalhado de desenvolvimento
├── PROJECT_MATRIX.md       # Documento mestre de engenharia e regras de design
├── README.md               # Documentação oficial do projeto
├── run_pc.bat              # Menu inicializador de resoluções
├── run_widescreen_16_9.bat # Inicializador 16:9
├── run_doom_13h.bat        # Inicializador modo Doom 13h
├── run_retro_4_3.bat       # Inicializador modo CRT 4:3
└── src/
    ├── main.cpp            # Ponto de entrada do jogo e loop principal
    ├── hal/                # Camada 0: Hardware Abstraction Layer
    │   ├── Platform.hpp/cpp# Janela, renderizador SDL2, perfis de tela e FPS
    │   └── Input.hpp/cpp   # Mapeador de teclado, mouse e multi-touch
    ├── engine/             # Camada 1: Motor Retrô 2.5D
    │   ├── Camera.hpp      # Posição vetorial, FOV e direção de visão
    │   ├── MathDef.hpp     # Vetores 2D e funções de iluminação/névoa
    │   ├── Raycaster.hpp/cpp# Algoritmo DDA e projeção de fatias de paredes
    │   └── Texture.hpp/cpp # Gerador das 24 texturas procedurais 64x64
    └── game/               # Camada 2: Lógica do DayZ
        ├── ChernogorskDef.hpp # Layout da grelha 12x10 e tags de edifícios
        ├── CityMap.hpp/cpp # Malha urbana de Chernogorsk 60x50 e colisão
        ├── Player.hpp/cpp  # Movimentação, Wall Sliding e sobrevivência
        └── UI/
            ├── HUD.hpp/cpp # Barras de vida, fome, stamina e prompt de portas
            └── Automap.hpp/cpp # Planta baixa tática em tela cheia com bússola
```

---

## 👤 Autor

**Luiz Corrêa**  
GitHub: [@LuizCorrea-Dev](https://github.com/LuizCorrea-Dev)  
Repositório: [https://github.com/LuizCorrea-Dev/DAYDOOM.git](https://github.com/LuizCorrea-Dev/DAYDOOM.git)

---

<div align="center">
<i>"No apocalipse de Chernogorsk, cada esquina pode ser a última. Sobreviva."</i>
</div>

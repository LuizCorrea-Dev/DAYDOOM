# 🧟 DAYDOOM 2.5D — DOCUMENTO MATRIZ DE ENGENHARIA & DESENVOLVIMENTO
> **Versão:** 1.0.0  
> **Conceito:** DayZ Reimaginado como um Demake Retrô em Estilo Doom Clássico (1993)  
> **Tecnologias Obrigatórias:** C++17 Nativo, SDL2, CMake  
> **Plataformas Alvo:** PC (Windows/Linux), Android Nativo (Android Studio / NDK), Web (WebAssembly via Emscripten)  

---

## 1. Visão Geral e Princípios Inegociáveis (Pilares)

1. **Estilo Visual e Estética:** 
   - Renderizador retrô **2.5D por Raycasting** (algoritmo DDA - Digital Differential Analysis, estilo Wolfenstein 3D / Doom inicial).
   - Renderização baseada em software (CPU desenha diretamente em um buffer de pixels `uint32_t screen[LARGURA * ALTURA]`).
   - Resolução interna canônica fixa (ex: **320x180** ou **480x270**, padrão 16:9 widescreen retrô), escalada pela GPU via SDL2 mantendo *Integer Scaling / Pixel Art Crisp* (sem borrões).
   - Visual pós-apocalíptico: ruas de asfalto rachado, prédios em ruínas, casas saqueadas, névoa escura/apocalíptica com efeito de iluminação decrescente por distância (*Distance Fog*).

2. **Alma do DayZ (Mecânicas Centrais):**
   - **Sobrevivência Realista:** Medidores de Vida, Fome, Sede, Sangramento e Stamina.
   - **Looting Urbano:** Latas de comida, garrafas de água, bandagens, armas brancas/fogo e caixas de munição espalhadas pelas ruas e dentro de construções.
   - **Inimigos (Infectados/Zumbis):** Não disparam magias ou bolas de fogo; atacam corporalmente em hordas, atraídos pela visão do jogador e barulho de disparos/corrida.
   - **Arma em Primeira Pessoa:** Sprites de armas animadas centralizadas na tela (estilo Doom).

3. **Diretriz de Portabilidade Absoluta:**
   - O código do jogo e da engine **nunca chamam APIs exclusivas do Windows, Android ou Web diretamente**.
   - Toda comunicação de hardware passa pela camada de abstração (HAL) apoiada na SDL2.
   - Zero dependências desnecessárias: sem bibliotecas inchadas, sem gerenciadores pesados.

---

## 2. Arquitetura em Camadas (Engine vs. Jogo)

O projeto é dividido estritamente em **3 Camadas Independentes**:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        CAMADA 2: O JOGO (DAYZ)                         │
│  - Player (Sobrevivência: Vida, Fome, Sede, Sangramento)               │
│  - Inventário & Loot (Comida, Bandagem, Armas, Munições)               │
│  - Zumbis & IA (Percepção sensorial, perseguição e ataque)             │
│  - Mapa Urbano (Ruas, Bairros, Casas saqueáveis, Carros quebrados)     │
│  - HUD Retrô & Controles Touch Mobile (Analógico virtual + Botões)     │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Usa os serviços da Engine
┌───────────────────────────────────▼────────────────────────────────────┐
│                    CAMADA 1: A ENGINE RETRÔ 2.5D                      │
│  - Raycaster (Algoritmo DDA para projeção de paredes 2.5D)             │
│  - Floor & Ceiling (Renderizador de asfalto e céu apocalíptico)        │
│  - SpriteRenderer (Ordenação por distância + Z-Buffer para entidades)  │
│  - TextureManager (Amostragem rápida de texturas em memória)           │
│  - AudioPlayer (Sons posicionais 2D de passos, gemidos e tiros)        │
│  - InputMapper (Traduz Teclado/Mouse/Touch para Ações Virtuais)        │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Usa a camada de abstração de hardware
┌───────────────────────────────────▼────────────────────────────────────┐
│                CAMADA 0: HAL (SDL2 & PLATFORM WRAPPER)                 │
│  - Criação da Janela e Framebuffer Texture (SDL2)                      │
│  - Multi-touch e Lifecycle do Android (Resume/Pause/Destroy)           │
│  - Main Loop com DeltaTime estável e travamento a 60 FPS               │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Padrões de Código e Reutilização

1. **Linguagem & Padrão:** C++17, focado em performance e simplicidade (evitar metaprogramação complexa ou hierarquias profundas de herança).
2. **Gerenciamento de Memória (Sem Garbage Collection / Sem Lag):**
   - **Zero alocações no loop de jogo (`new` / `malloc` no `update`/`render` são terminantemente proibidos).**
   - Entidades (Zumbis, Itens, Partículas) usam **Object Pooling** pré-alocado no carregamento do mapa.
3. **Resolução e Escalonamento:**
   - A CPU só desenha em um buffer linear: `uint32_t screenBuffer[WIDTH * HEIGHT]`.
   - A SDL2 transfere esse buffer para a tela com `SDL_UpdateTexture` e `SDL_RenderCopy`, preenchendo qualquer tela de celular, tablet ou monitor mantendo a proporção correta com barras pretas automáticas (*Letterboxing*).
4. **Controles Unificados (InputMapper):**
   - O código do jogo só conhece comandos abstratos:
     - `ACTION_MOVE_FORWARD`, `ACTION_MOVE_BACKWARD`, `ACTION_STRAFE_LEFT`, `ACTION_STRAFE_RIGHT`
     - `ACTION_TURN_LEFT`, `ACTION_TURN_RIGHT`, `ACTION_LOOK_DELTA_X`
     - `ACTION_ATTACK`, `ACTION_INTERACT_LOOT`, `ACTION_USE_BANDAGE`, `ACTION_RELOAD`
   - No **PC**: Mapeado para W, A, S, D + Mouse + E.
   - No **Android**: Mapeado para o joystick virtual e botões touch desenhados no HUD.

---

## 4. Estrutura de Pastas do Repositório

O projeto e todo o seu código-fonte, assets e builds ficam localizados dentro da pasta **`DAYDOOM/`**:

```text
games/
│
├── PROJECT_MATRIX.md            <-- Este documento (Guia definitivo do projeto)
├── IDEIAS/                      <-- Notas e conceitos preliminares
│
└── DAYDOOM/                     <-- DIRETÓRIO RAIZ DO PROJETO
    ├── CMakeLists.txt           <-- Build unificado para PC (Windows/Linux)
    ├── DEVLOG.md                <-- DIÁRIO DE BORDO DO DESENVOLVEDOR (Obrigatório)
    │
    ├── docs/                    <-- Documentação detalhada e capítulos do diário
    │   └── diary/               <-- Registros históricos das fases e soluções
    │
    ├── src/
    │   ├── main.cpp             <-- Ponto de entrada multiplataforma
    │   │
    │   ├── hal/                 <-- CAMADA 0: Hardware Abstraction Layer
    │   │   ├── Platform.hpp/.cpp<-- Inicialização da SDL2, Janela, Contexto, Loop
    │   │   ├── Audio.hpp/.cpp   <-- Emissão de áudio via SDL2
    │   │   └── Input.hpp/.cpp   <-- Teclado, Mouse e Multi-Touch do Android
    │   │
    │   ├── engine/              <-- CAMADA 1: Motor Retrô 2.5D
    │   │   ├── Raycaster.hpp/.cpp  <-- DDA de paredes, cálculo de altura e textura
    │   │   ├── FloorCeil.hpp/.cpp  <-- Renderizador de chão (asfalto) e teto (céu)
    │   │   ├── SpriteRenderer.hpp/.cpp <-- Projeção de sprites 2D com Z-buffer
    │   │   ├── Texture.hpp/.cpp <-- Banco e gerador de texturas de pixel
    │   │   ├── Camera.hpp/.cpp  <-- Posição, direção e plano de visão do jogador
    │   │   └── MathDef.hpp      <-- Estruturas leves (Vec2, Rect, Color)
    │   │
    │   └── game/                <-- CAMADA 2: DayDoom Gameplay
    │       ├── Player.hpp/.cpp  <-- Entidade do jogador, movimentação e física
    │       ├── Survival.hpp/.cpp<-- Lógica de Fome, Sede, Sangramento e Vida
    │       ├── Inventory.hpp/.cpp <-- Mochila, slots e consumo de suprimentos
    │       ├── Item.hpp/.cpp    <-- Latas, bandagens, armas, munições (drops)
    │       ├── Zombie.hpp/.cpp  <-- IA de infectados (Wander, Chase, Attack)
    │       ├── CityMap.hpp/.cpp <-- Matriz do mapa urbano (Ruas, Prédios, Portas)
    │       ├── Weapon.hpp/.cpp  <-- Armas em 1ª pessoa (Faca, Pistola, Escopeta)
    │       └── UI/
    │           ├── HUD.hpp/.cpp <-- Barras de Vida/Fome, Munição, Ícones
    │           └── TouchUI.hpp/.cpp <-- Analógico virtual e botões touch
    │
    ├── assets/                  <-- Texturas, sprites e sons (.wav, .png)
    │   ├── textures/            <-- Asfalto, tijolos, janelas quebradas, portas
    │   ├── sprites/             <-- Zumbis, comida, caixas de munição, armas
    │   └── sounds/              <-- Tiros, gemidos, passos, mastigação
    │
    ├── android/                 <-- Projeto Oficial do Android Studio
    │   ├── app/
    │   │   ├── src/main/
    │   │   │   ├── AndroidManifest.xml
    │   │   │   ├── java/org/libsdl/app/SDLActivity.java
    │   │   │   └── cpp/CMakeLists.txt  <-- Conecta o C++ via NDK
    │   │   └── build.gradle
    │   └── build.gradle
    │
    └── web/                     <-- Setup Emscripten (WebAssembly)
        ├── index.html           <-- Canvas HTML5 responsivo
        └── emscripten.sh        <-- Script de compilação para .wasm
```

---

## 5. Roteiro Passo a Passo de Execução (Fases)

### **Fase 1: Fundação & Pipeline Multiplataforma**
- [ ] Criar a estrutura dentro de `DAYDOOM/` com `CMakeLists.txt` raiz.
- [ ] Implementar a `Camada 0 (HAL)`:
  - Criação da janela redimensionável com renderizador SDL2 e textura de streaming.
  - Implementar o loop principal com medição precisa de DeltaTime e travamento a 60 FPS.
  - Testar com um padrão animado de pixels coloridos direto na tela no PC.
- [ ] Iniciar o primeiro registro no `DEVLOG.md`.

### **Fase 2: Motor Raycaster 2.5D (Ruas e Prédios)**
- [ ] Implementar `CityMap`: matriz de células representando ruas largas, calçadas e prédios.
- [ ] Implementar `Raycaster` com algoritmo DDA (cálculo de intersecção dos raios com as paredes).
- [ ] Gerar texturas procedurais retrô (tijolos escuros, asfalto cinza, parede de concreto, janelas) para que o jogo rode imediatamente sem precisar baixar arquivos externos.
- [ ] Adicionar iluminação com perda de intensidade por distância (*Distance Shading / Fog*).
- [ ] Implementar movimentação livre do `Player` (WASD + giro de câmera) com colisão nas paredes.
- [ ] Registrar desafios matemáticos e soluções no `DEVLOG.md`.

### **Fase 3: Renderização de Sprites com Z-Buffer**
- [ ] Implementar Z-Buffer de profundidade para que paredes escondam objetos que estejam atrás delas.
- [ ] Renderizar objetos no mapa: latas de comida, bandagens e carros destruídos como obstáculos.
- [ ] Renderizar arma do jogador em 1ª pessoa no canto inferior da tela com animação de disparo.
- [ ] Registrar algoritmo de ordenação de sprites e oclusão no `DEVLOG.md`.

### **Fase 4: Mecânicas de Sobrevivência do DayZ**
- [ ] Implementar sistema de status do jogador:
  - **Fome (Hunger):** Decresce com o tempo; correr acelera o consumo.
  - **Vida (Health):** Se a fome chegar a zero ou o zumbi atacar, a vida cai.
  - **Sangramento (Bleeding):** Chance de ocorrer ao ser ferido por zumbis; consome vida por segundo até usar uma bandagem.
  - **Stamina:** Usada para correr; recupera ao andar ou parar.
- [ ] Implementar sistema de **Looting**:
  - Andar até um item no chão e pressionar a tecla/botão de Interagir para recolher.
  - Usar comida para restaurar fome; usar bandagem para estancar sangramento.
- [ ] Registrar no `DEVLOG.md` como as mecânicas complexas do DayZ foram adaptadas para o loop simples e performático de Doom.

### **Fase 5: Os Infectados (Zumbis & IA)**
- [ ] Spawn de zumbis nas ruas e interiores de prédios.
- [ ] Máquina de Estados Finita (FSM):
  - `IDLE`: Zumbi parado ou perambulando lentamente.
  - `CHASE`: Ativado se o jogador entrar no cone de visão ou fizer barulho (tiro ou corrida próxima).
  - `ATTACK`: Ao se aproximar do jogador, desfere ataques corpo a corpo e causa sangramento.
- [ ] Sistema de combate: mirar e atirar nos zumbis, com feedback visual de dano e morte.
- [ ] Registrar no `DEVLOG.md` a lógica sensorial de IA (visão em cone e audição de disparos) em um mapa baseado em grid.

### **Fase 6: Interface Touch & Projeto Android Studio**
- [ ] Implementar `TouchUI.cpp`:
  - Analógico virtual translúcido no canto inferior esquerdo.
  - Botões touch no canto inferior direito: **Atirar (Gatilho)**, **Ação (Loot/Porta)**, **Bandagem/Comer**.
- [ ] Configurar a pasta `android/` com Gradle + NDK + CMake apontando para `src/`.
- [ ] Validar a compilação do APK no Android Studio e teste direto no celular físico.
- [ ] Registrar no `DEVLOG.md` os desafios da ponte JNI/NDK e a solução para multi-touch no Android.

### **Fase 7: Versão Web (WebAssembly) & Polimento Final**
- [ ] Compilar com Emscripten gerando `daydoom.html` e `daydoom.wasm`.
- [ ] Sons de passos, disparos, zumbis e mastigação integrados na HAL de áudio.
- [ ] Ajustes finais de balanceamento de sobrevivência e dificuldade.
- [ ] Fechamento e conclusões no `DEVLOG.md`.

---

## 6. Padrão Obrigatório de Documentação: Diário de Bordo (DevLog)

A cada implementação, alteração estrutural ou adição de mecânica, **a IA e o desenvolvedor devem obrigatoriamente registrar um capítulo no arquivo `DAYDOOM/DEVLOG.md`**.

### Estrutura Padrão de cada Registro no Diário:

```markdown
## [DATA - HORA] - Título da Implementação (Ex: Fase 1.1 - Inicialização da HAL e Loop 60 FPS)

### 1. O que foi Implementado
- Descrição clara dos arquivos criados ou modificados.
- Funcionalidades entregues nesta etapa.

### 2. Adaptação do DayZ para o Estilo Doom Clássico (Sem 3D Real)
- Como a mecânica clássica do DayZ foi traduzida para o motor 2.5D (ex: transposição de mundo aberto moderno para grid urbano retrô; substituição de modelos 3D por sprites planos 2D; adaptação de inventário complexo para slots minimalistas retrô).

### 3. Desafios Técnicos Encontrados
- Problemas com compilador, sincronização de tempo, ciclo de vida mobile, matemática do raycasting ou bugs visuais.

### 4. Soluções Criadas
- Como cada obstáculo foi superado tecnicamente no código C++.

### 5. Status Atual e Próximo Passo
- Estado do projeto testável e o que vem a seguir.

### 6. Checklist de Validação e Testes (PC / Mobile)
- [ ] Compilação sem avisos/erros.
- [ ] Taxa de quadros estável (60 FPS cravados).
- [ ] Teste de movimentação e colisão em paredes.
- [ ] Validação de controles (Teclado/Mouse no PC ou Touch no Android).
```

---

## 7. Regras de Ouro e Diretrizes Inegociáveis

1. **Localização:** Todo o projeto reside obrigatoriamente dentro de **`DAYDOOM/`**.
2. **Estética 2.5D Retrô (Sem 3D Real):** Sem modelos poligonais 3D modernos nem motores prontos pesados. Toda a perspectiva vem da projeção de raios por software (Raycasting DDA) desenhada em um buffer de pixels linear.
3. **Desempenho:** Zero alocações no loop principal (`new`/`malloc` proibidos no `update`/`render`).
4. **Registro Contínuo:** Nenhuma fase é considerada concluída sem o seu respectivo registro documentado no `DEVLOG.md`.
5. **Multiplataforma:** Código de lógica e engine 100% puro e agnóstico de sistema operacional.

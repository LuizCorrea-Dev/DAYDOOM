#define SDL_MAIN_HANDLED
#include "hal/Platform.hpp"
#include "hal/Input.hpp"
#include "engine/Texture.hpp"
#include "engine/Raycaster.hpp"
#include "game/CityMap.hpp"
#include "game/Player.hpp"
#include "game/UI/HUD.hpp"
#include "game/UI/Automap.hpp"
#include <iostream>
#include <string>

#if defined(__ANDROID__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// Ponto de entrada compatível com SDL2
int main(int argc, char* argv[]) {
    SDL_SetMainReady();

    std::cout << "========================================" << std::endl;
    std::cout << "  DAYDOOM 2.5D - DayZ Retro Demake      " << std::endl;
    std::cout << "  C++17 + SDL2 Multiplataforma          " << std::endl;
    std::cout << "========================================" << std::endl;

    // Detecta se o usuário solicitou o perfil Retrô 4:3 ou Doom 13h via argumento de linha de comando
    DayDoom::HAL::DisplayProfile profile = DayDoom::HAL::DisplayProfile::WIDESCREEN_16_9; // Padrão: Celular Landscape e PC Widescreen
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--43" || arg == "--crt") {
            profile = DayDoom::HAL::DisplayProfile::RETRO_4_3;
            std::cout << "[MAIN] Modo Retro CRT 4:3 (320x240) ativado!" << std::endl;
        } else if (arg == "--doom" || arg == "--13h") {
            profile = DayDoom::HAL::DisplayProfile::CLASSIC_DOOM_13H;
            std::cout << "[MAIN] Modo Doom 13h (320x200) ativado!" << std::endl;
        }
    }

    // 1. Inicializa a Camada 0: HAL com perfil adaptativo
    DayDoom::HAL::Platform platform;
    int initialWinW = (profile == DayDoom::HAL::DisplayProfile::RETRO_4_3) ? 640 : 960;
    int initialWinH = (profile == DayDoom::HAL::DisplayProfile::RETRO_4_3) ? 480 : 540;

    if (!platform.init("DayDoom 2.5D - DayZ Retro Demake", profile, initialWinW, initialWinH, false)) {
        std::cerr << "[MAIN] Falha crítica ao inicializar a plataforma." << std::endl;
        return 1;
    }

    DayDoom::HAL::Input input;

    // 2. Inicializa a Camada 1: Engine Retrô 2.5D com a resolução ativa
    DayDoom::Engine::TextureManager textures;
    DayDoom::Engine::Raycaster raycaster(platform.getScreenWidth(), platform.getScreenHeight());

    // 3. Inicializa a Camada 2: O Jogo (DayZ Chernogorsk Demake)
    DayDoom::Game::CityMap cityMap;
    DayDoom::Game::Player player(cityMap.getSpawnPosition().x, cityMap.getSpawnPosition().y);
    player.getCamera().setAngle(cityMap.getSpawnAngle());
    DayDoom::Game::UI::HUD hud(platform.getScreenWidth(), platform.getScreenHeight());
    DayDoom::Game::UI::Automap automap;

    bool showAutomap = false;
    float totalTime = 0.0f;

    std::cout << "[MAIN] Planta Baixa de Chernogorsk inicializada (Grelha Proporcional 12x10 expandida em 60x50)!" << std::endl;
    std::cout << "[MAIN] Spawn do Jogador no Setor 1 (Praia do Mar Baltico): (" 
              << cityMap.getSpawnPosition().x << ", " << cityMap.getSpawnPosition().y << ") olhando para o Norte!" << std::endl;
    std::cout << "[MAIN] Pressione [M] ou [Tab] a qualquer momento para abrir o Mapa Tatico em tela cheia!" << std::endl;
    std::cout << "[MAIN] Loop principal rodando a 60 FPS com resolucao de " 
              << platform.getScreenWidth() << "x" << platform.getScreenHeight() << "!" << std::endl;

    // 4. Loop Principal de Jogo
    while (platform.isRunning()) {
        platform.beginFrame();
        float deltaTime = platform.getDeltaTime();
        totalTime += deltaTime;

        // Processa eventos de hardware (Teclado, Mouse, Toque Android, Fechar Janela, F11 Fullscreen)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }
                if (event.key.keysym.sym == SDLK_F11) {
                    platform.toggleFullscreen();
                }
            }
            input.handleEvent(event);
        }

        // Atualização de estados de entrada
        input.update();

        // Alterna a exibição da Planta Baixa Tática com a tecla [M] ou [Tab]
        if (input.isActionJustPressed(DayDoom::HAL::Action::TOGGLE_MAP)) {
            showAutomap = !showAutomap;
            std::cout << "[MAP] Planta Baixa de Chernogorsk " << (showAutomap ? "ABERTA" : "FECHADA") << std::endl;
        }

        // Atualização da lógica do jogador, física e mecânicas de sobrevivência do DayZ
        // O jogador continua atualizando mesmo no mapa ou pode navegar
        player.update(deltaTime, input, cityMap);

        uint32_t* screenBuffer = platform.getScreenBuffer();

        if (showAutomap) {
            // Renderização da Planta Baixa Tática em Tela Cheia com Texturas Reais
            automap.render(screenBuffer, platform.getScreenWidth(), platform.getScreenHeight(), cityMap, player, textures, totalTime);
        } else {
            // Renderização 2.5D por Raycasting (Céu, Chão texturizado e Paredes de Edifícios/Muros/Portas)
            raycaster.render(screenBuffer, player.getCamera(), cityMap, textures);

            // Verifica se o jogador está olhando diretamente para uma porta próxima
            int targetCellX = static_cast<int>(player.getCamera().pos.x + player.getCamera().dir.x * 1.3f);
            int targetCellY = static_cast<int>(player.getCamera().pos.y + player.getCamera().dir.y * 1.3f);
            bool nearDoor = cityMap.isDoor(targetCellX, targetCellY);

            // Renderização do HUD de Sobrevivência (Vida, Fome, Stamina, Sangramento, Setor e Mira)
            const char* sectorName = cityMap.getSectorName(player.getCamera().pos.x, player.getCamera().pos.y);
            hud.render(screenBuffer, player, platform.getFPS(), sectorName, nearDoor);
        }

        // Apresenta o buffer na tela via escalonador nítido da SDL2
        platform.endFrame();
    }

    platform.shutdown();
    std::cout << "[MAIN] Jogo finalizado com sucesso." << std::endl;
    return 0;
}

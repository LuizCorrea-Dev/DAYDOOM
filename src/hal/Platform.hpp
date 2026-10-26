#pragma once

#include <cstdint>
#include <string>
#include <vector>

#if defined(__ANDROID__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace DayDoom::HAL {

// Perfis de Resolução e Proporção de Tela
enum class DisplayProfile {
    WIDESCREEN_16_9,   // 480x270: PC e Celulares em Modo Paisagem (Horizontal)
    RETRO_4_3,         // 320x240: Proporcao Retro CRT 4:3 classica
    CLASSIC_DOOM_13H   // 320x200: Modo 13h MS-DOS puro do Doom classico de 1993
};

class Platform {
public:
    Platform();
    ~Platform();

    // Inicialização com perfil de tela adaptável
    bool init(
        const std::string& title,
        DisplayProfile profile = DisplayProfile::WIDESCREEN_16_9,
        int windowWidth = 960,
        int windowHeight = 540,
        bool fullscreen = false
    );

    void shutdown();

    bool processEvents();
    void beginFrame();
    void endFrame();

    // Alternar tela cheia dinamicamente
    void toggleFullscreen();

    // Acesso direto ao buffer de pixels da CPU
    uint32_t* getScreenBuffer() { return m_screenBuffer.data(); }
    void clearScreen(uint32_t color = 0xFF000000);

    // Tempo e desempenho
    float getDeltaTime() const { return m_deltaTime; }
    float getFPS() const { return m_fps; }
    bool isRunning() const { return m_running; }

    // Resoluções canônicas ativas
    int getScreenWidth() const { return m_canonicalWidth; }
    int getScreenHeight() const { return m_canonicalHeight; }
    DisplayProfile getProfile() const { return m_profile; }

    SDL_Window* getSDLWindow() const { return m_window; }
    SDL_Renderer* getSDLRenderer() const { return m_renderer; }

private:
    DisplayProfile m_profile = DisplayProfile::WIDESCREEN_16_9;
    int m_canonicalWidth = 480;
    int m_canonicalHeight = 270;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_framebufferTexture = nullptr;

    std::vector<uint32_t> m_screenBuffer;

    bool m_running = false;
    bool m_isFullscreen = false;
    uint64_t m_lastTime = 0;
    float m_deltaTime = 0.016f;
    float m_fps = 60.0f;
    int m_frameCount = 0;
    float m_fpsTimer = 0.0f;
};

} // namespace DayDoom::HAL

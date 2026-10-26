#include "Platform.hpp"
#include <iostream>
#include <algorithm>

namespace DayDoom::HAL {

Platform::Platform() {
}

Platform::~Platform() {
    shutdown();
}

bool Platform::init(
    const std::string& title,
    DisplayProfile profile,
    int windowWidth,
    int windowHeight,
    bool fullscreen
) {
    m_profile = profile;

    // 1. Configuração da Resolução Canônica baseada no Perfil Selecionado
    switch (m_profile) {
        case DisplayProfile::RETRO_4_3:
            // Perfil Retro CRT 4:3
            m_canonicalWidth = 320;
            m_canonicalHeight = 240;
            break;

        case DisplayProfile::CLASSIC_DOOM_13H:
            // Modo 13h original do Doom (1993)
            m_canonicalWidth = 320;
            m_canonicalHeight = 200;
            break;

        case DisplayProfile::WIDESCREEN_16_9:
        default:
            // Perfil Widescreen para PC e Celulares na Horizontal (Landscape)
            m_canonicalWidth = 480;
            m_canonicalHeight = 270;
            break;
    }

    // 2. Travar a orientação do Celular (Android/iOS) obrigatoriamente em Paisagem (Horizontal)
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

    // 3. Inicialização dos subsistemas da SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        std::cerr << "[HAL] Falha ao inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Configuração das flags da janela
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    #if defined(__ANDROID__)
    // No celular, sempre entra em modo imersivo de tela cheia horizontal
    windowFlags |= SDL_WINDOW_FULLSCREEN;
    m_isFullscreen = true;
    #else
    if (fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        m_isFullscreen = true;
    }
    #endif

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        windowFlags
    );

    if (!m_window) {
        std::cerr << "[HAL] Falha ao criar janela SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // 4. Criação do Renderer com VSync
    m_renderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!m_renderer) {
        std::cerr << "[HAL] Fallback para renderizador por software: " << SDL_GetError() << std::endl;
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (!m_renderer) {
        std::cerr << "[HAL] Erro fatal ao criar SDL_Renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // 5. Adaptação Dinâmica à Tela:
    // SDL_RenderSetLogicalSize garante que o buffer canônico se adapte a qualquer tela
    // (celular deitado, monitor 1080p, 4K ou tela 320x240) mantendo a proporção com Letterboxing automático!
    SDL_RenderSetLogicalSize(m_renderer, m_canonicalWidth, m_canonicalHeight);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // 0 = Nearest Neighbor (Pixel art nítido sem borrões)

    // 6. Alocação do Buffer de Memória da CPU
    m_screenBuffer.resize(m_canonicalWidth * m_canonicalHeight, 0xFF000000);

    // 7. Textura de Streaming na GPU
    m_framebufferTexture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_canonicalWidth,
        m_canonicalHeight
    );

    if (!m_framebufferTexture) {
        std::cerr << "[HAL] Falha ao criar SDL_Texture de streaming: " << SDL_GetError() << std::endl;
        return false;
    }

    m_running = true;
    m_lastTime = SDL_GetPerformanceCounter();

    std::cout << "[HAL] Plataforma adaptada à tela! Resolução Ativa: " 
              << m_canonicalWidth << "x" << m_canonicalHeight 
              << " (" << (m_profile == DisplayProfile::RETRO_4_3 ? "Retro 4:3" : (m_profile == DisplayProfile::CLASSIC_DOOM_13H ? "Classic Doom 13h" : "Widescreen 16:9")) 
              << ")" << std::endl;

    return true;
}

void Platform::shutdown() {
    if (m_framebufferTexture) {
        SDL_DestroyTexture(m_framebufferTexture);
        m_framebufferTexture = nullptr;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
    m_running = false;
}

void Platform::toggleFullscreen() {
    m_isFullscreen = !m_isFullscreen;
    uint32_t flags = m_isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_SetWindowFullscreen(m_window, flags);
}

bool Platform::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_running = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_running = false;
            } else if (event.key.keysym.sym == SDLK_F11) {
                toggleFullscreen();
            }
        }

        #if defined(__ANDROID__)
        if (event.type == SDL_APP_WILLENTERBACKGROUND) {
            std::cout << "[HAL] Celular minimizado / tela bloqueada" << std::endl;
        } else if (event.type == SDL_APP_DIDENTERFOREGROUND) {
            std::cout << "[HAL] Celular reativado no jogo" << std::endl;
        }
        #endif
    }
    return m_running;
}

void Platform::beginFrame() {
    uint64_t currentTime = SDL_GetPerformanceCounter();
    uint64_t frequency = SDL_GetPerformanceFrequency();
    m_deltaTime = static_cast<float>(currentTime - m_lastTime) / static_cast<float>(frequency);
    m_lastTime = currentTime;

    if (m_deltaTime > 0.1f) {
        m_deltaTime = 0.1f;
    }

    m_frameCount++;
    m_fpsTimer += m_deltaTime;
    if (m_fpsTimer >= 1.0f) {
        m_fps = static_cast<float>(m_frameCount) / m_fpsTimer;
        m_frameCount = 0;
        m_fpsTimer = 0.0f;
    }
}

void Platform::endFrame() {
    // 1. Atualiza a textura da GPU com o buffer de pixels da CPU
    SDL_UpdateTexture(
        m_framebufferTexture,
        nullptr,
        m_screenBuffer.data(),
        m_canonicalWidth * sizeof(uint32_t)
    );

    // 2. A GPU desenha o buffer adaptado à tela com Letterboxing preservando a proporção exata
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_framebufferTexture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
}

void Platform::clearScreen(uint32_t color) {
    std::fill(m_screenBuffer.begin(), m_screenBuffer.end(), color);
}

} // namespace DayDoom::HAL

#pragma once

#include "../Player.hpp"
#include <cstdint>
#include <string>

namespace DayDoom::Game::UI {

class HUD {
public:
    HUD(int screenWidth, int screenHeight);

    // Desenha as barras de sobrevivência do DayZ, mira, setor ativo e prompt de portas
    void render(uint32_t* screenBuffer, const Player& player, float fps, const char* locationName = nullptr, bool nearDoor = false);

private:
    int m_screenWidth;
    int m_screenHeight;

    void drawRect(uint32_t* screenBuffer, int x, int y, int w, int h, uint32_t color);
    void drawBar(uint32_t* screenBuffer, int x, int y, int w, int h, float percent, uint32_t fillColor, uint32_t bgColor);
    void drawCrosshair(uint32_t* screenBuffer);
    void drawChar(uint32_t* screenBuffer, int x, int y, char c, uint32_t color);
    void drawText(uint32_t* screenBuffer, int x, int y, const std::string& text, uint32_t color);
};

} // namespace DayDoom::Game::UI

#pragma once

#include "../Player.hpp"
#include "../CityMap.hpp"
#include "../../engine/Texture.hpp"
#include <cstdint>
#include <string>

namespace DayDoom::Game::UI {

class Automap {
public:
    Automap();

    // Renderiza a planta baixa tática 64x64 de Chernogorsk fiel à referência 2D
    void render(
        uint32_t* screenBuffer,
        int screenWidth,
        int screenHeight,
        const CityMap& map,
        const Player& player,
        const Engine::TextureManager& textures,
        float time
    );

private:
    void drawRect(uint32_t* buf, int sw, int sh, int x, int y, int w, int h, uint32_t color);
    void drawBorder(uint32_t* buf, int sw, int sh, int x, int y, int w, int h, uint32_t color);
    void drawLine(uint32_t* buf, int sw, int sh, int x0, int y0, int x1, int y1, uint32_t color);
    void drawChar(uint32_t* buf, int sw, int sh, int x, int y, char c, uint32_t color);
    void drawText(uint32_t* buf, int sw, int sh, int x, int y, const std::string& text, uint32_t color);
    int  drawWrappedText(uint32_t* buf, int sw, int sh, int x, int y, int maxWidth, const std::string& text, uint32_t color);
};

} // namespace DayDoom::Game::UI
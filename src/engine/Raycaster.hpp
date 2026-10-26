#pragma once

#include "Camera.hpp"
#include "Texture.hpp"
#include "../game/CityMap.hpp"
#include <vector>
#include <cstdint>

namespace DayDoom::Engine {

class Raycaster {
public:
    Raycaster(int screenWidth, int screenHeight);

    // Renderiza um quadro completo de paredes, chão e céu diretamente no buffer de pixels
    void render(
        uint32_t* screenBuffer,
        const Camera& camera,
        const Game::CityMap& map,
        const TextureManager& textures
    );

    // Acesso ao Z-Buffer para oclusão de sprites de zumbis e itens
    const std::vector<float>& getZBuffer() const { return m_zBuffer; }

private:
    int m_screenWidth;
    int m_screenHeight;
    std::vector<float> m_zBuffer;

    void renderFloorAndCeiling(
        uint32_t* screenBuffer,
        const Camera& camera,
        const Game::CityMap& map,
        const TextureManager& textures
    );

    void renderWalls(
        uint32_t* screenBuffer,
        const Camera& camera,
        const Game::CityMap& map,
        const TextureManager& textures
    );
};

} // namespace DayDoom::Engine

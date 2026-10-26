#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace DayDoom::Engine {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }

    float lengthSq() const { return x * x + y * y; }
    float length() const { return std::sqrt(lengthSq()); }

    Vec2 normalized() const {
        float l = length();
        return l > 0.0001f ? Vec2{x / l, y / l} : Vec2{0.0f, 0.0f};
    }
};

// Funções para manipulação rápida de cores ARGB8888 e Shading/Fog
inline uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           (static_cast<uint32_t>(b));
}

// Aplica iluminação ambiente e névoa atmosférica de Chernogorsk (preserva texturas nítidas)
inline uint32_t applyFog(uint32_t color, float distance, float maxDist = 38.0f) {
    constexpr float MIN_LIGHT = 0.32f; // Mantém pelo menos 32% de iluminação para texturas nunca virarem blocos pretos
    float t = distance / maxDist;
    if (t > 1.0f) t = 1.0f;

    // Curva de atenuação suave estilo Doom clássico
    float factor = (1.0f - t) * (1.0f - MIN_LIGHT) + MIN_LIGHT;

    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * factor);
    uint8_t g = static_cast<uint8_t>(((color >> 8)  & 0xFF) * factor);
    uint8_t b = static_cast<uint8_t>((color & 0xFF)         * factor);

    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           (static_cast<uint32_t>(b));
}

// Escurece levemente paredes viradas para o eixo horizontal (sombra de face 3D)
inline uint32_t applySideShade(uint32_t color, int side) {
    if (side == 0) return color; // Face vertical

    // Face horizontal com 82% de luminosidade (dá relevo volumétrico sem perder detalhes)
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = ((color >> 16) & 0xFF) * 82 / 100;
    uint8_t g = ((color >> 8)  & 0xFF) * 82 / 100;
    uint8_t b = (color & 0xFF)         * 82 / 100;

    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8)  |
           (static_cast<uint32_t>(b));
}

} // namespace DayDoom::Engine

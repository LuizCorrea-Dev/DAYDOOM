#pragma once

#include "MathDef.hpp"
#include <cmath>

namespace DayDoom::Engine {

class Camera {
public:
    Vec2 pos{3.5f, 3.5f};
    Vec2 dir{1.0f, 0.0f};
    Vec2 plane{0.0f, 0.66f}; // Plano da câmera (determina o FOV de ~66 graus estilo Doom clássico)

    Camera() = default;
    Camera(float x, float y, float angleRadians = 0.0f) {
        pos = {x, y};
        setAngle(angleRadians);
    }

    void setAngle(float angleRadians) {
        dir.x = std::cos(angleRadians);
        dir.y = std::sin(angleRadians);
        // Plano perpendicular com magnitude de 0.66
        plane.x = -dir.y * 0.66f;
        plane.y = dir.x * 0.66f;
    }

    void rotate(float angleRadians) {
        float cosA = std::cos(angleRadians);
        float sinA = std::sin(angleRadians);

        // Rotação do vetor de direção
        float oldDirX = dir.x;
        dir.x = dir.x * cosA - dir.y * sinA;
        dir.y = oldDirX * sinA + dir.y * cosA;

        // Rotação do plano da câmera
        float oldPlaneX = plane.x;
        plane.x = plane.x * cosA - plane.y * sinA;
        plane.y = oldPlaneX * sinA + plane.y * cosA;
    }
};

} // namespace DayDoom::Engine

#pragma once

#include "../engine/Camera.hpp"
#include "../hal/Input.hpp"
#include "CityMap.hpp"

namespace DayDoom::Game {

class Player {
public:
    Player(float startX = 3.5f, float startY = 3.5f);

    void update(float deltaTime, const HAL::Input& input, CityMap& map);

    const Engine::Camera& getCamera() const { return m_camera; }
    Engine::Camera& getCamera() { return m_camera; }

    // Status de sobrevivência do DayZ
    float getHealth() const { return m_health; }
    float getHunger() const { return m_hunger; }
    float getStamina() const { return m_stamina; }
    bool isBleeding() const { return m_bleeding; }

    void takeDamage(float amount);
    void applyBandage();
    void eatFood(float nutrition = 35.0f);

private:
    Engine::Camera m_camera;

    float m_walkSpeed = 3.2f;
    float m_runSpeed = 5.2f;
    float m_turnSpeed = 2.4f;
    float m_radius = 0.28f; // Raio de colisão com paredes

    // Status do DayZ
    float m_health = 100.0f;
    float m_hunger = 100.0f;   // 100 = satisfeito, 0 = morrendo de fome
    float m_stamina = 100.0f;  // Consumido ao correr
    bool m_bleeding = false;
};

} // namespace DayDoom::Game

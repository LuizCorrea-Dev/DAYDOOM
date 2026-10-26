#include "Player.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace DayDoom::Game {

Player::Player(float startX, float startY)
    : m_camera(startX, startY, 0.0f) {
}

void Player::update(float deltaTime, const HAL::Input& input, CityMap& map) {
    // 1. Rotação da Câmera (Teclas de seta/QE ou Mouse Delta ou Toque)
    float turnInput = input.getTurnAxis();
    int mouseDeltaX = input.getMouseDeltaX();

    if (mouseDeltaX != 0) {
        float mouseSensitivity = 0.003f;
        m_camera.rotate(static_cast<float>(mouseDeltaX) * mouseSensitivity);
    } else if (turnInput != 0.0f) {
        m_camera.rotate(turnInput * m_turnSpeed * deltaTime);
    }

    // 2. Velocidade e Stamina do DayZ
    bool isRunning = input.isActionPressed(HAL::Action::RUN) && m_stamina > 10.0f;
    float currentSpeed = isRunning ? m_runSpeed : m_walkSpeed;

    // Consumo / recuperação de Stamina
    if (isRunning && (input.getMoveAxisX() != 0.0f || input.getMoveAxisY() != 0.0f)) {
        m_stamina = std::max(0.0f, m_stamina - 20.0f * deltaTime);
        m_hunger = std::max(0.0f, m_hunger - 0.8f * deltaTime); // Correr dá mais fome
    } else {
        m_stamina = std::min(100.0f, m_stamina + 15.0f * deltaTime);
        m_hunger = std::max(0.0f, m_hunger - 0.2f * deltaTime); // Fome passiva
    }

    // Efeitos da Fome extrema e Sangramento
    if (m_hunger <= 0.0f) {
        m_health = std::max(0.0f, m_health - 3.0f * deltaTime); // Dano de inanição
    }
    if (m_bleeding) {
        m_health = std::max(0.0f, m_health - 2.5f * deltaTime); // Sangramento ativo
    }

    // 3. Vetor de Movimento
    float moveForward = input.getMoveAxisY();
    float moveStrafe = input.getMoveAxisX();

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (moveForward != 0.0f) {
        moveX += m_camera.dir.x * moveForward;
        moveY += m_camera.dir.y * moveForward;
    }
    if (moveStrafe != 0.0f) {
        // Vetor perpendicular para strafe
        moveX += -m_camera.dir.y * moveStrafe;
        moveY += m_camera.dir.x * moveStrafe;
    }

    // Normaliza o vetor para não andar mais rápido na diagonal
    float moveLenSq = moveX * moveX + moveY * moveY;
    if (moveLenSq > 0.0001f) {
        float len = std::sqrt(moveLenSq);
        moveX = (moveX / len) * currentSpeed * deltaTime;
        moveY = (moveY / len) * currentSpeed * deltaTime;
    }

    // 4. Colisão com paredes com suporte a deslizamento (Wall Sliding)
    // Testa movimento no eixo X
    float newPosX = m_camera.pos.x + moveX;
    if (map.isWalkable(newPosX, m_camera.pos.y, m_radius)) {
        m_camera.pos.x = newPosX;
    }

    // Testa movimento no eixo Y separadamente
    float newPosY = m_camera.pos.y + moveY;
    if (map.isWalkable(m_camera.pos.x, newPosY, m_radius)) {
        m_camera.pos.y = newPosY;
    }

    // 5. Interação com o Mundo: Abrir e Fechar Portas com 'F' / 'Space'
    if (input.isActionJustPressed(HAL::Action::INTERACT_LOOT)) {
        // Célula diretamente na frente do jogador (até 1.4 unidades à frente)
        int targetCellX = static_cast<int>(m_camera.pos.x + m_camera.dir.x * 1.4f);
        int targetCellY = static_cast<int>(m_camera.pos.y + m_camera.dir.y * 1.4f);

        if (map.isDoor(targetCellX, targetCellY)) {
            bool wasOpen = map.isDoorOpen(targetCellX, targetCellY);
            map.toggleDoor(targetCellX, targetCellY);
            std::cout << "[GAME] Porta " << (wasOpen ? "fechando..." : "abrindo...") 
                      << " em: (" << targetCellX << ", " << targetCellY << ")" << std::endl;
        }
    }

    // 6. Usar suprimentos rápidos do inventário (Teclas 1 e 2)
    if (input.isActionJustPressed(HAL::Action::USE_BANDAGE)) {
        applyBandage();
    }
    if (input.isActionJustPressed(HAL::Action::USE_FOOD)) {
        eatFood();
    }
}

void Player::takeDamage(float amount) {
    m_health = std::max(0.0f, m_health - amount);
    m_bleeding = true; // Ataque de zumbi causa sangramento
    std::cout << "[GAME] Jogador ferido! Vida: " << m_health << " | Sangrando: SIM" << std::endl;
}

void Player::applyBandage() {
    if (m_bleeding) {
        m_bleeding = false;
        std::cout << "[GAME] Bandagem aplicada com sucesso! Sangramento estancado." << std::endl;
    } else {
        std::cout << "[GAME] O jogador não está sangrando." << std::endl;
    }
}

void Player::eatFood(float nutrition) {
    m_hunger = std::min(100.0f, m_hunger + nutrition);
    std::cout << "[GAME] Comida consumida! Nível de fome restaurado para: " << m_hunger << "%" << std::endl;
}

} // namespace DayDoom::Game

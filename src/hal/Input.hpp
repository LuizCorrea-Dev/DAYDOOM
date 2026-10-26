#pragma once

#include <cstdint>
#include <unordered_map>

#if defined(__ANDROID__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

namespace DayDoom::HAL {

enum class Action {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    STRAFE_LEFT,
    STRAFE_RIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    RUN,
    ATTACK,
    INTERACT_LOOT,
    USE_BANDAGE,
    USE_FOOD,
    RELOAD,
    TOGGLE_MAP
};

struct TouchFinger {
    int64_t id = -1;
    float x = 0.0f; // 0.0 a 1.0 (normalizado)
    float y = 0.0f;
    bool active = false;
};

class Input {
public:
    Input();

    void update();
    void handleEvent(const SDL_Event& event);

    // Estado das ações
    bool isActionPressed(Action action) const;
    bool isActionJustPressed(Action action) const;

    // Eixos analógicos (-1.0f a 1.0f)
    float getMoveAxisX() const; // -1 (esquerda), +1 (direita)
    float getMoveAxisY() const; // -1 (trás), +1 (frente)
    float getTurnAxis() const;  // -1 (virar esquerda), +1 (virar direita)

    // Entrada por toque no celular
    const TouchFinger& getLeftStickTouch() const { return m_leftStickTouch; }
    const TouchFinger& getRightLookTouch() const { return m_rightLookTouch; }
    void setVirtualStickOutput(float x, float y) { m_virtualStickX = x; m_virtualStickY = y; }
    void setVirtualButton(Action action, bool pressed);

    // Mouse Delta
    int getMouseDeltaX() const { return m_mouseDeltaX; }
    int getMouseDeltaY() const { return m_mouseDeltaY; }

private:
    const uint8_t* m_keystates = nullptr;
    bool m_actionStates[16] = {false};
    bool m_prevActionStates[16] = {false};

    int m_mouseDeltaX = 0;
    int m_mouseDeltaY = 0;

    float m_virtualStickX = 0.0f;
    float m_virtualStickY = 0.0f;

    TouchFinger m_leftStickTouch;
    TouchFinger m_rightLookTouch;
};

} // namespace DayDoom::HAL

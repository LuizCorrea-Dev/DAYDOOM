#include "Input.hpp"

namespace DayDoom::HAL {

Input::Input() {
    m_keystates = SDL_GetKeyboardState(nullptr);
}

void Input::update() {
    for (size_t i = 0; i < 16; ++i) {
        m_prevActionStates[i] = m_actionStates[i];
    }
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;

    // Atualiza teclas do PC
    if (m_keystates) {
        m_actionStates[static_cast<size_t>(Action::MOVE_FORWARD)]  = m_keystates[SDL_SCANCODE_W] || m_keystates[SDL_SCANCODE_UP];
        m_actionStates[static_cast<size_t>(Action::MOVE_BACKWARD)] = m_keystates[SDL_SCANCODE_S] || m_keystates[SDL_SCANCODE_DOWN];
        m_actionStates[static_cast<size_t>(Action::STRAFE_LEFT)]   = m_keystates[SDL_SCANCODE_A];
        m_actionStates[static_cast<size_t>(Action::STRAFE_RIGHT)]  = m_keystates[SDL_SCANCODE_D];
        m_actionStates[static_cast<size_t>(Action::TURN_LEFT)]     = m_keystates[SDL_SCANCODE_LEFT] || m_keystates[SDL_SCANCODE_Q];
        m_actionStates[static_cast<size_t>(Action::TURN_RIGHT)]    = m_keystates[SDL_SCANCODE_RIGHT] || m_keystates[SDL_SCANCODE_E];
        m_actionStates[static_cast<size_t>(Action::RUN)]           = m_keystates[SDL_SCANCODE_LSHIFT] || m_keystates[SDL_SCANCODE_RSHIFT];
        m_actionStates[static_cast<size_t>(Action::INTERACT_LOOT)] = m_keystates[SDL_SCANCODE_F] || m_keystates[SDL_SCANCODE_SPACE];
        m_actionStates[static_cast<size_t>(Action::USE_BANDAGE)]   = m_keystates[SDL_SCANCODE_1];
        m_actionStates[static_cast<size_t>(Action::USE_FOOD)]      = m_keystates[SDL_SCANCODE_2];
        m_actionStates[static_cast<size_t>(Action::RELOAD)]        = m_keystates[SDL_SCANCODE_R];
        m_actionStates[static_cast<size_t>(Action::TOGGLE_MAP)]    = m_keystates[SDL_SCANCODE_M] || m_keystates[SDL_SCANCODE_TAB];
    }

    // Botões do mouse
    uint32_t mouseState = SDL_GetMouseState(nullptr, nullptr);
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        m_actionStates[static_cast<size_t>(Action::ATTACK)] = true;
    }
}

void Input::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        m_mouseDeltaX += event.motion.xrel;
        m_mouseDeltaY += event.motion.yrel;
    }

    // Tratamento de toque multi-touch no Android
    if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION) {
        // Lado esquerdo da tela (X < 0.5): controle de movimento
        if (event.tfinger.x < 0.5f) {
            m_leftStickTouch.id = event.tfinger.fingerId;
            m_leftStickTouch.x = event.tfinger.x;
            m_leftStickTouch.y = event.tfinger.y;
            m_leftStickTouch.active = true;
        } else {
            // Lado direito da tela (X >= 0.5): controle de câmera/olhar
            m_rightLookTouch.id = event.tfinger.fingerId;
            m_rightLookTouch.x = event.tfinger.x;
            m_rightLookTouch.y = event.tfinger.y;
            m_rightLookTouch.active = true;
        }
    } else if (event.type == SDL_FINGERUP) {
        if (event.tfinger.fingerId == m_leftStickTouch.id) {
            m_leftStickTouch.active = false;
            m_virtualStickX = 0.0f;
            m_virtualStickY = 0.0f;
        }
        if (event.tfinger.fingerId == m_rightLookTouch.id) {
            m_rightLookTouch.active = false;
        }
    }
}

bool Input::isActionPressed(Action action) const {
    size_t idx = static_cast<size_t>(action);
    return idx < 16 ? m_actionStates[idx] : false;
}

bool Input::isActionJustPressed(Action action) const {
    size_t idx = static_cast<size_t>(action);
    return idx < 16 ? (m_actionStates[idx] && !m_prevActionStates[idx]) : false;
}

float Input::getMoveAxisX() const {
    float axis = 0.0f;
    if (isActionPressed(Action::STRAFE_RIGHT)) axis += 1.0f;
    if (isActionPressed(Action::STRAFE_LEFT))  axis -= 1.0f;
    if (m_virtualStickX != 0.0f) axis = m_virtualStickX;
    return axis;
}

float Input::getMoveAxisY() const {
    float axis = 0.0f;
    if (isActionPressed(Action::MOVE_FORWARD))  axis += 1.0f;
    if (isActionPressed(Action::MOVE_BACKWARD)) axis -= 1.0f;
    if (m_virtualStickY != 0.0f) axis = m_virtualStickY;
    return axis;
}

float Input::getTurnAxis() const {
    float axis = 0.0f;
    if (isActionPressed(Action::TURN_RIGHT)) axis += 1.0f;
    if (isActionPressed(Action::TURN_LEFT))  axis -= 1.0f;
    return axis;
}

void Input::setVirtualButton(Action action, bool pressed) {
    size_t idx = static_cast<size_t>(action);
    if (idx < 16) {
        m_actionStates[idx] = pressed;
    }
}

} // namespace DayDoom::HAL

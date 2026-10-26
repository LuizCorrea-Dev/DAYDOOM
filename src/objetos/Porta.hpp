#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include "../engine/MathDef.hpp"
#include "../engine/Texture.hpp"
#include "../game/ChernogorskDef.hpp"

namespace DayDoom::Objetos {

enum class EstadoPorta {
    FECHADA = 0,
    ABRINDO = 1,
    ABERTA = 2,
    FECHANDO = 3
};

using DoorState = EstadoPorta;

struct PortaRayHit {
    bool hit = false;
    float dist = 1e9f;
    float u = 0.0f;     // Coordenada horizontal da textura [0, 1] ao longo da folha
    int side = 0;       // Orientacao para sombreamento (0 = vertical, 1 = horizontal)
    int texID = 0;      // ID da textura atingida (tipo da porta ou DOOR_FRAME nos fechamentos)
};

using DoorRayHit = PortaRayHit;

class Porta {
public:
    static constexpr float ESPESSURA = 0.05f; // Espessura da folha = 1/20 da largura do bloco
    static constexpr float THICKNESS = ESPESSURA;
    static constexpr float VELOCIDADE_ABERTURA = 1.6f; // Gira de 0° a 90° em ~0.6s

    Porta() = default;
    Porta(int x, int y, int tipo = Game::TYPE_DOOR_WOOD);

    // Configuracao geometrica do vao e dobradica
    void configurarDobradica(
        float hx, float hy,
        float bDirX, float bDirY,
        float sDir,
        float comprimento = 0.96f,
        bool vertical = false
    );

    // Ciclo de atualizacao e animacao
    void update(float deltaTime);
    void toggle();
    void abrir();
    void fechar();

    void open() { abrir(); }
    void close() { fechar(); }

    // Consultas de estado
    bool isPassavel() const { return m_progressoAbertura > 0.40f; }
    bool isWalkable() const { return isPassavel(); }

    bool isAberta() const { return m_progressoAbertura > 0.5f; }
    bool isOpen() const { return isAberta(); }

    bool isFechada() const { return m_progressoAbertura <= 0.02f; }
    bool isClosed() const { return isFechada(); }

    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getTipo() const { return m_tipo; }
    int getType() const { return m_tipo; }
    void setTipo(int tipo) { m_tipo = tipo; }
    void setType(int tipo) { m_tipo = tipo; }

    EstadoPorta getEstado() const { return m_estado; }
    DoorState getState() const { return m_estado; }
    void setEstado(EstadoPorta estado) { m_estado = estado; }
    void setState(DoorState estado) { m_estado = estado; }

    float getProgressoAbertura() const { return m_progressoAbertura; }
    float getOpenProgress() const { return m_progressoAbertura; }

    float getAnguloAtual() const { return m_anguloAtual; }
    float getCurrentAngle() const { return m_anguloAtual; }

    bool isVertical() const { return m_isVertical; }
    float getHingeX() const { return m_hingeX; }
    float getHingeY() const { return m_hingeY; }
    float getBaseDirX() const { return m_baseDirX; }
    float getBaseDirY() const { return m_baseDirY; }
    float getSwingDir() const { return m_swingDir; }
    float getComprimento() const { return m_comprimento; }
    float getLength() const { return m_comprimento; }

    // Geometria 3D: Obtem os 4 vertices do paralelepipedo solido da folha
    void getVertices(
        Engine::Vec2& c0,
        Engine::Vec2& c1,
        Engine::Vec2& c2,
        Engine::Vec2& c3
    ) const;

    // Teste de intersecao de raio contra as 4 faces da porta solida 3D
    // (Frente, Tras, Fechamento Perpendicular da Ponta, Fechamento Perpendicular da Dobradica)
    PortaRayHit intersectRay(
        float rayStartX, float rayStartY,
        float rayDirX, float rayDirY
    ) const;

private:
    int m_x = 0;
    int m_y = 0;
    int m_tipo = Game::TYPE_DOOR_WOOD;
    EstadoPorta m_estado = EstadoPorta::FECHADA;
    float m_progressoAbertura = 0.0f; // [0.0f, 1.0f]
    float m_anguloAtual = 0.0f;       // [0.0f, PI/2]
    bool m_isVertical = false;

    float m_hingeX = 0.0f;
    float m_hingeY = 0.0f;
    float m_baseDirX = 1.0f;
    float m_baseDirY = 0.0f;
    float m_swingDir = 1.0f;
    float m_comprimento = 0.96f;
};

using Door = Porta;

} // namespace DayDoom::Objetos

namespace DayDoom::Game {
    using Porta = Objetos::Porta;
    using Door = Objetos::Door;
    using DoorState = Objetos::DoorState;
    using EstadoPorta = Objetos::EstadoPorta;
    using PortaRayHit = Objetos::PortaRayHit;
    using DoorRayHit = Objetos::DoorRayHit;
}

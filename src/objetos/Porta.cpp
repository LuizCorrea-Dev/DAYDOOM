#include "Porta.hpp"

namespace DayDoom::Objetos {

Porta::Porta(int x, int y, int tipo)
    : m_x(x), m_y(y), m_tipo(tipo) {
}

void Porta::configurarDobradica(
    float hx, float hy,
    float bDirX, float bDirY,
    float sDir,
    float comprimento,
    bool vertical
) {
    m_hingeX = hx;
    m_hingeY = hy;
    m_baseDirX = bDirX;
    m_baseDirY = bDirY;
    m_swingDir = sDir;
    m_comprimento = comprimento;
    m_isVertical = vertical;
}

void Porta::update(float deltaTime) {
    if (m_estado == EstadoPorta::ABRINDO) {
        m_progressoAbertura += VELOCIDADE_ABERTURA * deltaTime;
        if (m_progressoAbertura >= 1.0f) {
            m_progressoAbertura = 1.0f;
            m_estado = EstadoPorta::ABERTA;
        }
    } else if (m_estado == EstadoPorta::FECHANDO) {
        m_progressoAbertura -= VELOCIDADE_ABERTURA * deltaTime;
        if (m_progressoAbertura <= 0.0f) {
            m_progressoAbertura = 0.0f;
            m_estado = EstadoPorta::FECHADA;
        }
    }
    m_anguloAtual = m_progressoAbertura * 1.57079632679f; // 0° a 90° em radianos
}

void Porta::toggle() {
    if (m_estado == EstadoPorta::FECHADA || m_estado == EstadoPorta::FECHANDO) {
        abrir();
    } else {
        fechar();
    }
}

void Porta::abrir() {
    m_estado = EstadoPorta::ABRINDO;
}

void Porta::fechar() {
    m_estado = EstadoPorta::FECHANDO;
}

void Porta::getVertices(
    Engine::Vec2& c0,
    Engine::Vec2& c1,
    Engine::Vec2& c2,
    Engine::Vec2& c3
) const {
    float angle = m_swingDir * m_anguloAtual;
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    float ux = cosA * m_baseDirX - sinA * m_baseDirY;
    float uy = sinA * m_baseDirX + cosA * m_baseDirY;

    float nx = -uy;
    float ny = ux;

    float T = ESPESSURA;
    float hT = 0.5f * T;
    float L = m_comprimento;

    c0.x = m_hingeX - hT * nx;
    c0.y = m_hingeY - hT * ny;

    c1.x = c0.x + L * ux;
    c1.y = c0.y + L * uy;

    c2.x = c1.x + T * nx;
    c2.y = c1.y + T * ny;

    c3.x = c0.x + T * nx;
    c3.y = c0.y + T * ny;
}

PortaRayHit Porta::intersectRay(
    float rayStartX, float rayStartY,
    float rayDirX, float rayDirY
) const {
    PortaRayHit result;

    Engine::Vec2 c0, c1, c2, c3;
    getVertices(c0, c1, c2, c3);

    auto testSegment = [&](float ax, float ay, float bx, float by, int texID) {
        float vx = bx - ax;
        float vy = by - ay;
        float det = rayDirY * vx - rayDirX * vy;
        if (std::abs(det) < 1e-6f) return;

        float delX = ax - rayStartX;
        float delY = ay - rayStartY;
        float t = (delY * vx - delX * vy) / det;
        float s = (rayDirX * delY - rayDirY * delX) / det;

        if (t > 0.05f && t < result.dist && s >= 0.0f && s <= 1.0f) {
            result.hit = true;
            result.dist = t;
            result.u = s;
            result.side = (std::abs(vy) > std::abs(vx)) ? 0 : 1;
            result.texID = texID;
        }
    };

    // Face 1: Lado frontal da folha (C0 -> C1)
    testSegment(c0.x, c0.y, c1.x, c1.y, m_tipo);

    // Face 2: Lado traseiro da folha (C3 -> C2)
    testSegment(c3.x, c3.y, c2.x, c2.y, m_tipo);

    // Face 3: Fechamento perpendicular na ponta da folha (C1 -> C2) - Largura ESPESSURA (0.05)
    testSegment(c1.x, c1.y, c2.x, c2.y, Engine::TextureManager::DOOR_FRAME);

    // Face 4: Fechamento perpendicular na dobradiça da folha (C0 -> C3) - Largura ESPESSURA (0.05)
    testSegment(c0.x, c0.y, c3.x, c3.y, Engine::TextureManager::DOOR_FRAME);

    return result;
}

} // namespace DayDoom::Objetos

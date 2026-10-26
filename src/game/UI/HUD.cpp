#include "HUD.hpp"
#include "../../engine/MathDef.hpp"
#include <algorithm>

namespace DayDoom::Game::UI {

HUD::HUD(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight) {
}

void HUD::render(uint32_t* screenBuffer, const Player& player, float fps, const char* locationName, bool nearDoor) {
    // 1. Mira Retrô no Centro da Tela
    drawCrosshair(screenBuffer);

    // Prompt dinâmico de interação ao se aproximar de qualquer porta/portão
    if (nearDoor) {
        drawRect(screenBuffer, m_screenWidth / 2 - 36, m_screenHeight / 2 + 8, 72, 10, Engine::makeColor(15, 18, 24, 220));
        drawText(screenBuffer, m_screenWidth / 2 - 33, m_screenHeight / 2 + 10, "[F]: ABRIR PORTA", Engine::makeColor(255, 220, 40));
    }

    // 2. Painel Inferior de Sobrevivência (Estilo HUD do DayZ)
    int hudY = m_screenHeight - 26;
    int hudHeight = 22;

    // Fundo translúcido para as barras de status
    drawRect(screenBuffer, 10, hudY, 210, hudHeight, Engine::makeColor(15, 15, 15, 200));

    // Barra de Vida (Health)
    float hpPercent = player.getHealth() / 100.0f;
    uint32_t hpColor = hpPercent > 0.3f ? Engine::makeColor(45, 190, 45) : Engine::makeColor(210, 30, 30);
    drawText(screenBuffer, 16, hudY + 4, "HP", hpColor);
    drawBar(screenBuffer, 32, hudY + 4, 60, 6, hpPercent, hpColor, Engine::makeColor(50, 20, 20));

    // Barra de Fome (Hunger)
    float hungerPercent = player.getHunger() / 100.0f;
    uint32_t hungerColor = hungerPercent > 0.25f ? Engine::makeColor(210, 160, 30) : Engine::makeColor(230, 70, 20);
    drawText(screenBuffer, 102, hudY + 4, "FOOD", hungerColor);
    drawBar(screenBuffer, 134, hudY + 4, 60, 6, hungerPercent, hungerColor, Engine::makeColor(50, 40, 15));

    // Barra de Stamina (Vigor)
    float staminaPercent = player.getStamina() / 100.0f;
    uint32_t staminaColor = Engine::makeColor(45, 135, 225);
    drawText(screenBuffer, 16, hudY + 12, "STM", staminaColor);
    drawBar(screenBuffer, 38, hudY + 13, 80, 4, staminaPercent, staminaColor, Engine::makeColor(20, 30, 50));

    // Alerta de Sangramento (Bleeding)
    if (player.isBleeding()) {
        drawText(screenBuffer, 134, hudY + 12, "[BLEEDING!]", Engine::makeColor(255, 30, 30));
    }

    // Exibição do Setor de Chernogorsk no topo
    if (locationName) {
        drawText(screenBuffer, 10, 8, locationName, Engine::makeColor(240, 210, 80));
    } else {
        drawText(screenBuffer, 10, 8, "CHERNOGORSK 2.5D", Engine::makeColor(180, 180, 180));
    }
    drawText(screenBuffer, m_screenWidth - 70, 8, "FPS: " + std::to_string(static_cast<int>(fps)), Engine::makeColor(100, 220, 100));
    drawText(screenBuffer, 10, 18, "WASD: MOVER | MOUSE: OLHAR | F: PORTA | M: MAPA", Engine::makeColor(140, 140, 140));
}

void HUD::drawRect(uint32_t* screenBuffer, int x, int y, int w, int h, uint32_t color) {
    int startX = std::max(0, x);
    int endX = std::min(m_screenWidth, x + w);
    int startY = std::max(0, y);
    int endY = std::min(m_screenHeight, y + h);

    for (int cy = startY; cy < endY; ++cy) {
        for (int cx = startX; cx < endX; ++cx) {
            screenBuffer[cy * m_screenWidth + cx] = color;
        }
    }
}

void HUD::drawBar(uint32_t* screenBuffer, int x, int y, int w, int h, float percent, uint32_t fillColor, uint32_t bgColor) {
    percent = std::clamp(percent, 0.0f, 1.0f);
    int filledW = static_cast<int>(static_cast<float>(w) * percent);

    // Fundo da barra
    drawRect(screenBuffer, x, y, w, h, bgColor);
    // Preenchimento ativo
    if (filledW > 0) {
        drawRect(screenBuffer, x, y, filledW, h, fillColor);
    }
}

void HUD::drawCrosshair(uint32_t* screenBuffer) {
    int cx = m_screenWidth / 2;
    int cy = m_screenHeight / 2;
    uint32_t crossColor = Engine::makeColor(230, 230, 230, 200);

    // Linhas horizontal e vertical de 5 pixels
    for (int i = -3; i <= 3; ++i) {
        if (i != 0) {
            screenBuffer[cy * m_screenWidth + (cx + i)] = crossColor;
            screenBuffer[(cy + i) * m_screenWidth + cx] = crossColor;
        }
    }
}

static uint16_t getGlyphPattern(char c) {
    switch (c) {
        case '0': return 0b111110011001111;
        case '1': return 0b010010010010010;
        case '2': return 0b111001111100111;
        case '3': return 0b111001011001111;
        case '4': return 0b101101111001001;
        case '5': return 0b111100111001111;
        case '6': return 0b111100111101111;
        case '7': return 0b111001001010010;
        case '8': return 0b111101111101111;
        case '9': return 0b111101111001111;
        case ':': return 0b000010000010000;
        case 'A': return 0b010101111101101;
        case 'B': return 0b110101110101110;
        case 'C': return 0b011100100100011;
        case 'D': return 0b110101101101110;
        case 'E': return 0b111100110100111;
        case 'F': return 0b111100110100100;
        case 'G': return 0b011100101101011;
        case 'H': return 0b101101111101101;
        case 'I': return 0b111010010010111;
        case 'J': return 0b001001001101010;
        case 'K': return 0b101101110101101;
        case 'L': return 0b100100100100111;
        case 'M': return 0b101111101101101;
        case 'N': return 0b101111101101101;
        case 'O': return 0b111101101101111;
        case 'P': return 0b111101111100100;
        case 'Q': return 0b011101101011001;
        case 'R': return 0b110101110101101;
        case 'S': return 0b011100010001110;
        case 'T': return 0b111010010010010;
        case 'U': return 0b101101101101111;
        case 'V': return 0b101101101010010;
        case 'W': return 0b101101101111101;
        case 'X': return 0b101101010101101;
        case 'Y': return 0b101101010010010;
        case 'Z': return 0b111001010100111;
        case '!': return 0b010010010000010;
        case '[': return 0b011010010010011;
        case ']': return 0b110010010010110;
        case '-': return 0b000000111000000;
        case '.': return 0b000000000000010;
        case '/': return 0b001001010100100;
        case ' ': return 0;
        default: return 0b111101101101111;
    }
}

// Fonte bitmap mínima 4x6 embutida para números, letras e símbolos
void HUD::drawChar(uint32_t* screenBuffer, int startX, int startY, char c, uint32_t color) {
    uint16_t glyph = getGlyphPattern(c);
    if (glyph == 0) return;

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            int bit = (row * 3) + col;
            if ((glyph >> (14 - bit)) & 1) {
                int px = startX + col;
                int py = startY + row;
                if (px >= 0 && px < m_screenWidth && py >= 0 && py < m_screenHeight) {
                    screenBuffer[py * m_screenWidth + px] = color;
                }
            }
        }
    }
}

void HUD::drawText(uint32_t* screenBuffer, int x, int y, const std::string& text, uint32_t color) {
    int curX = x;
    for (char c : text) {
        drawChar(screenBuffer, curX, y, c, color);
        curX += 4; // 3 pixels de caractere + 1 de espaçamento
    }
}

} // namespace DayDoom::Game::UI

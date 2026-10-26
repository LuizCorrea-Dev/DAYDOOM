#include "Automap.hpp"
#include "../../engine/MathDef.hpp"
#include "../ChernogorskDef.hpp"
#include <cmath>
#include <algorithm>
#include <string>

namespace DayDoom::Game::UI {

Automap::Automap() {
}

static uint16_t getAutomapGlyph(char c) {
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
        case '&': return 0b010101010101010;
        case '^': return 0b010101000000000;
        case '[': return 0b011010010010011;
        case ']': return 0b110010010010110;
        case '(': return 0b010100100100010;
        case ')': return 0b010001001001010;
        case '-': return 0b000000111000000;
        case '.': return 0b000000000000010;
        case ',': return 0b000000000010100;
        case '/': return 0b001001010100100;
        case '!': return 0b010010010000010;
        case ' ': return 0;
        default: return 0b111101101101111;
    }
}

void Automap::drawChar(uint32_t* buf, int sw, int sh, int x, int y, char c, uint32_t color) {
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    uint16_t glyph = getAutomapGlyph(c);
    if (glyph == 0) return;

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            int bit = (row * 3) + col;
            if ((glyph >> (14 - bit)) & 1) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < sw && py >= 0 && py < sh) {
                    buf[py * sw + px] = color;
                }
            }
        }
    }
}

void Automap::drawText(uint32_t* buf, int sw, int sh, int x, int y, const std::string& text, uint32_t color) {
    int curX = x;
    for (char c : text) {
        drawChar(buf, sw, sh, curX, y, c, color);
        curX += 4;
    }
}

int Automap::drawWrappedText(uint32_t* buf, int sw, int sh, int x, int y, int maxWidth, const std::string& text, uint32_t color) {
    int curX = x;
    int curY = y;
    int maxCharsPerLine = std::max(8, maxWidth / 4);

    std::string word;
    int lineChars = 0;

    for (size_t i = 0; i <= text.size(); ++i) {
        char c = (i < text.size()) ? text[i] : ' ';
        if (c == ' ') {
            if (!word.empty()) {
                if (lineChars + static_cast<int>(word.size()) > maxCharsPerLine && lineChars > 0) {
                    curX = x;
                    curY += 7;
                    lineChars = 0;
                }
                drawText(buf, sw, sh, curX, curY, word, color);
                curX += static_cast<int>(word.size() + 1) * 4;
                lineChars += static_cast<int>(word.size()) + 1;
                word.clear();
            }
        } else {
            word.push_back(c);
        }
    }
    return curY + 8;
}

void Automap::drawRect(uint32_t* buf, int sw, int sh, int x, int y, int w, int h, uint32_t color) {
    int x1 = std::max(0, x);
    int x2 = std::min(sw, x + w);
    int y1 = std::max(0, y);
    int y2 = std::min(sh, y + h);

    for (int cy = y1; cy < y2; ++cy) {
        for (int cx = x1; cx < x2; ++cx) {
            buf[cy * sw + cx] = color;
        }
    }
}

void Automap::drawBorder(uint32_t* buf, int sw, int sh, int x, int y, int w, int h, uint32_t color) {
    drawRect(buf, sw, sh, x, y, w, 1, color);
    drawRect(buf, sw, sh, x, y + h - 1, w, 1, color);
    drawRect(buf, sw, sh, x, y, 1, h, color);
    drawRect(buf, sw, sh, x + w - 1, y, 1, h, color);
}

void Automap::drawLine(uint32_t* buf, int sw, int sh, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < sw && y0 >= 0 && y0 < sh) {
            buf[y0 * sw + x0] = color;
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

// Mistura uma cor de matiz tático com a amostra real da textura para legibilidade em 64x64
static uint32_t blendTacticalTint(uint32_t texColor, uint8_t tr, uint8_t tg, uint8_t tb, int tintWeight) {
    uint8_t r = (texColor >> 16) & 0xFF;
    uint8_t g = (texColor >> 8)  & 0xFF;
    uint8_t b = texColor & 0xFF;

    int inv = 256 - tintWeight;
    uint8_t outR = static_cast<uint8_t>((r * inv + tr * tintWeight) >> 8);
    uint8_t outG = static_cast<uint8_t>((g * inv + tg * tintWeight) >> 8);
    uint8_t outB = static_cast<uint8_t>((b * inv + tb * tintWeight) >> 8);

    return Engine::makeColor(outR, outG, outB);
}

void Automap::render(
    uint32_t* screenBuffer,
    int screenWidth,
    int screenHeight,
    const CityMap& map,
    const Player& player,
    const Engine::TextureManager& textures,
    float time
) {
    // 1. Fundo Tático (Cinza-azulado escuro idêntico à borda da planta de referência)
    drawRect(screenBuffer, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, Engine::makeColor(18, 24, 32));

    // 2. Cálculo da Escala Quadrada 64x64
    int mapW = map.getWidth();   // 64
    int mapH = map.getHeight();  // 64

    int panelReservedW = (screenWidth <= 360) ? 102 : 148;
    int availW = screenWidth - panelReservedW - 14;
    int availH = screenHeight - 12;

    float cellSize = std::min(
        static_cast<float>(availW) / static_cast<float>(mapW),
        static_cast<float>(availH) / static_cast<float>(mapH)
    );
    if (cellSize < 1.0f) cellSize = 1.0f;

    int mapPixelW = static_cast<int>(mapW * cellSize);
    int mapPixelH = static_cast<int>(mapH * cellSize);

    int mapX = 6;
    int mapY = (screenHeight - mapPixelH) / 2;

    // Moldura dupla do mapa
    drawBorder(screenBuffer, screenWidth, screenHeight, mapX - 2, mapY - 2, mapPixelW + 4, mapPixelH + 4, Engine::makeColor(65, 85, 110));
    drawBorder(screenBuffer, screenWidth, screenHeight, mapX - 1, mapY - 1, mapPixelW + 2, mapPixelH + 2, Engine::makeColor(25, 35, 48));

    // 3. Renderização da Malha 64x64 com World-Space UV Tiling e Silhuetas Limpas
    for (int cy = 0; cy < mapH; ++cy) {
        int py1 = mapY + static_cast<int>(cy * cellSize);
        int py2 = mapY + static_cast<int>((cy + 1) * cellSize);
        int cellH = std::max(1, py2 - py1);

        for (int cx = 0; cx < mapW; ++cx) {
            int px1 = mapX + static_cast<int>(cx * cellSize);
            int px2 = mapX + static_cast<int>((cx + 1) * cellSize);
            int cellW = std::max(1, px2 - px1);

            int cell = map.getCell(cx, cy);
            int floorType = map.getFloorType(cx, cy);

            int texID = Engine::TextureManager::ASPHALT;
            uint8_t tintR = 90, tintG = 100, tintB = 110;
            int tintStrength = 110; // Equilíbrio entre textura real e nitidez de planta 2D

            if (cell == TYPE_EMPTY) {
                switch (floorType) {
                    case FLOOR_WATER:
                        texID = Engine::TextureManager::WATER;
                        tintR = 35; tintG = 78; tintB = 118; tintStrength = 140;
                        break;
                    case FLOOR_SAND:
                        texID = Engine::TextureManager::SAND;
                        tintR = 215; tintG = 192; tintB = 135; tintStrength = 150;
                        break;
                    case FLOOR_CONCRETE:
                        texID = Engine::TextureManager::CONC_FLOOR;
                        tintR = 115; tintG = 124; tintB = 132; tintStrength = 130;
                        break;
                    case FLOOR_WOOD:
                        texID = Engine::TextureManager::WOOD_FLOOR;
                        tintR = 135; tintG = 105; tintB = 75; tintStrength = 120;
                        break;
                    case FLOOR_RAILROAD:
                        texID = Engine::TextureManager::RAILROAD;
                        tintR = 95; tintG = 88; tintB = 82; tintStrength = 110;
                        break;
                    case FLOOR_ASPHALT:
                    default:
                        texID = Engine::TextureManager::ASPHALT;
                        tintR = 82; tintG = 92; tintB = 102; tintStrength = 150;
                        break;
                }
            } else {
                switch (cell) {
                    case TYPE_WALL_BRICK:
                        texID = Engine::TextureManager::BRICK_DARK;
                        tintR = 155; tintG = 95; tintB = 78; tintStrength = 120;
                        break;
                    case TYPE_WALL_CONCRETE:
                        texID = Engine::TextureManager::CONCRETE;
                        tintR = 135; tintG = 142; tintB = 150; tintStrength = 120;
                        break;
                    case TYPE_WALL_METAL:
                        texID = Engine::TextureManager::RUSTY_FENCE;
                        tintR = 120; tintG = 130; tintB = 142; tintStrength = 130;
                        break;
                    case TYPE_CONTAINER_RED:
                        texID = Engine::TextureManager::CONTAINER_RED;
                        tintR = 185; tintG = 60; tintB = 50; tintStrength = 140;
                        break;
                    case TYPE_CONTAINER_BLU:
                        texID = Engine::TextureManager::CONTAINER_BLUE;
                        tintR = 55; tintG = 105; tintB = 165; tintStrength = 140;
                        break;
                    case TYPE_DOOR_WOOD:
                        texID = Engine::TextureManager::WOOD_DOOR;
                        tintR = 220; tintG = 165; tintB = 55; tintStrength = 140;
                        break;
                    case TYPE_DOOR_SLIDING:
                        texID = Engine::TextureManager::DOOR_SLIDING;
                        tintR = 100; tintG = 190; tintB = 230; tintStrength = 140;
                        break;
                    case TYPE_DOOR_HOSPITAL:
                        texID = Engine::TextureManager::DOOR_HOSPITAL;
                        tintR = 235; tintG = 80; tintB = 80; tintStrength = 150;
                        break;
                    case TYPE_GATE_LARGE:
                        texID = Engine::TextureManager::GATE_IRON;
                        tintR = 240; tintG = 200; tintB = 50; tintStrength = 150;
                        break;
                    case TYPE_FENCE_WIRE:
                        texID = Engine::TextureManager::FENCE_WIRE;
                        tintR = 95; tintG = 105; tintB = 115; tintStrength = 130;
                        break;
                    case TYPE_MURO_SOVIET:
                        texID = Engine::TextureManager::MURO_SOVIET;
                        tintR = 108; tintG = 116; tintB = 126; tintStrength = 140;
                        break;
                    case TYPE_MURO_STONE:
                        texID = Engine::TextureManager::MURO_STONE;
                        tintR = 115; tintG = 110; tintB = 102; tintStrength = 130;
                        break;
                    case TYPE_WINDOW_GLASS:
                        texID = Engine::TextureManager::STORE_FRONT;
                        tintR = 130; tintG = 205; tintB = 235; tintStrength = 150;
                        break;
                    case TYPE_WALL_HOSPITAL:
                        texID = Engine::TextureManager::HOSPITAL_WALL;
                        tintR = 215; tintG = 220; tintB = 225; tintStrength = 150;
                        break;
                    case TYPE_HANGAR_BLUE:
                        texID = Engine::TextureManager::HANGAR_BLUE;
                        tintR = 88; tintG = 128; tintB = 172; tintStrength = 145;
                        break;
                    case TYPE_POLICE_EMBLEM:
                        texID = Engine::TextureManager::POLICE_EMBLEM;
                        tintR = 70; tintG = 130; tintB = 210; tintStrength = 150;
                        break;
                    case TYPE_WATER_BARRIER:
                        texID = Engine::TextureManager::WATER;
                        tintR = 25; tintG = 58; tintB = 95; tintStrength = 160;
                        break;
                    default:
                        texID = Engine::TextureManager::BRICK_DARK;
                        break;
                }
            }

            // Preenche a célula usando coordenadas UV contínuas no espaço do mapa
            for (int ly = 0; ly < cellH; ++ly) {
                int sy = py1 + ly;
                if (sy < 0 || sy >= screenHeight) continue;

                int texV = ((cy * 8) + (ly * 8) / cellH) & (Engine::TextureManager::TEX_SIZE - 1);

                for (int lx = 0; lx < cellW; ++lx) {
                    int sx = px1 + lx;
                    if (sx < 0 || sx >= screenWidth) continue;

                    int texU = ((cx * 8) + (lx * 8) / cellW) & (Engine::TextureManager::TEX_SIZE - 1);
                    uint32_t rawTex = textures.getPixel(texID, texU, texV);
                    uint32_t finalColor = blendTacticalTint(rawTex, tintR, tintG, tintB, tintStrength);

                    // Destaque visual dos dormentes da ferrovia (Y == 20)
                    if (cell == TYPE_EMPTY && floorType == FLOOR_RAILROAD) {
                        if (ly == 0 || ly == cellH - 1 || (lx == 0 && (cx % 2 == 0))) {
                            finalColor = Engine::makeColor(55, 50, 46);
                        }
                    }
                    // Linha branca da espuma das ondas entre a praia e o mar
                    if (cell == TYPE_EMPTY && floorType == FLOOR_SAND) {
                        if (map.getFloorType(cx, cy + 1) == FLOOR_WATER && ly == cellH - 1) {
                            finalColor = Engine::makeColor(225, 238, 248);
                        }
                    }

                    screenBuffer[sy * screenWidth + sx] = finalColor;
                }
            }

            // 4. Contorno Arquitetónico Inteligente (Apenas na borda externa das paredes)
            if (isWallType(cell)) {
                uint32_t outlineColor = Engine::makeColor(24, 28, 34);
                if (!isWallType(map.getCell(cx, cy - 1))) drawRect(screenBuffer, screenWidth, screenHeight, px1, py1, cellW, 1, outlineColor);
                if (!isWallType(map.getCell(cx, cy + 1))) drawRect(screenBuffer, screenWidth, screenHeight, px1, py2 - 1, cellW, 1, outlineColor);
                if (!isWallType(map.getCell(cx - 1, cy))) drawRect(screenBuffer, screenWidth, screenHeight, px1, py1, 1, cellH, outlineColor);
                if (!isWallType(map.getCell(cx + 1, cy))) drawRect(screenBuffer, screenWidth, screenHeight, px2 - 1, py1, 1, cellH, outlineColor);
            }

            // 5. Destaque para Portas Interativas (Piscando em Amarelo/Âmbar)
            if (map.isDoor(cx, cy)) {
                uint32_t doorColor = ((static_cast<int>(time * 4.0f) % 2) == 0)
                    ? Engine::makeColor(255, 225, 45)
                    : Engine::makeColor(215, 130, 20);
                if (cellW >= 3 && cellH >= 3) {
                    drawBorder(screenBuffer, screenWidth, screenHeight, px1, py1, cellW, cellH, doorColor);
                } else {
                    drawRect(screenBuffer, screenWidth, screenHeight, px1, py1, cellW, cellH, doorColor);
                }
            }
        }
    }

    // 6. Grelha de Quadrantes Táticos Sobreposta (Inspirada na planta original)
    uint32_t gridLineColor = Engine::makeColor(190, 210, 230);
    for (int g = 8; g < 64; g += 8) {
        int gx = mapX + static_cast<int>(g * cellSize);
        int gy = mapY + static_cast<int>(g * cellSize);

        // Linhas verticais e horizontais semitransparentes (20% opacidade)
        for (int y = mapY; y < mapY + mapPixelH; ++y) {
            if (gx >= 0 && gx < screenWidth && y >= 0 && y < screenHeight) {
                uint32_t bg = screenBuffer[y * screenWidth + gx];
                screenBuffer[y * screenWidth + gx] = blendTacticalTint(bg, 185, 205, 225, 36);
            }
        }
        for (int x = mapX; x < mapX + mapPixelW; ++x) {
            if (x >= 0 && x < screenWidth && gy >= 0 && gy < screenHeight) {
                uint32_t bg = screenBuffer[gy * screenWidth + x];
                screenBuffer[gy * screenWidth + x] = blendTacticalTint(bg, 185, 205, 225, 36);
            }
        }
    }

    // 7. Ponto de Localização do Jogador e Seta de Visão
    float playerMapX = static_cast<float>(mapX) + player.getCamera().pos.x * cellSize;
    float playerMapY = static_cast<float>(mapY) + player.getCamera().pos.y * cellSize;
    int pX = static_cast<int>(playerMapX);
    int pY = static_cast<int>(playerMapY);

    float dirX = player.getCamera().dir.x;
    float dirY = player.getCamera().dir.y;

    int arrowEndX = static_cast<int>(playerMapX + dirX * 9.0f);
    int arrowEndY = static_cast<int>(playerMapY + dirY * 9.0f);
    drawLine(screenBuffer, screenWidth, screenHeight, pX, pY, arrowEndX, arrowEndY, Engine::makeColor(255, 255, 255));

    float perpDirX = -dirY;
    float perpDirY = dirX;
    int wing1X = static_cast<int>(playerMapX + dirX * 6.0f + perpDirX * 3.0f);
    int wing1Y = static_cast<int>(playerMapY + dirY * 6.0f + perpDirY * 3.0f);
    int wing2X = static_cast<int>(playerMapX + dirX * 6.0f - perpDirX * 3.0f);
    int wing2Y = static_cast<int>(playerMapY + dirY * 6.0f - perpDirY * 3.0f);
    drawLine(screenBuffer, screenWidth, screenHeight, arrowEndX, arrowEndY, wing1X, wing1Y, Engine::makeColor(255, 255, 255));
    drawLine(screenBuffer, screenWidth, screenHeight, arrowEndX, arrowEndY, wing2X, wing2Y, Engine::makeColor(255, 255, 255));

    uint32_t beaconColor = ((static_cast<int>(time * 6.0f) % 2) == 0)
        ? Engine::makeColor(255, 230, 40)
        : Engine::makeColor(30, 255, 90);
    drawRect(screenBuffer, screenWidth, screenHeight, pX - 2, pY - 2, 5, 5, Engine::makeColor(10, 10, 15));
    drawRect(screenBuffer, screenWidth, screenHeight, pX - 1, pY - 1, 3, 3, beaconColor);

    // 8. Painel Tático Lateral
    int panelX = mapX + mapPixelW + 6;
    int panelY = mapY;
    int panelW = screenWidth - panelX - 6;
    int panelH = mapPixelH;

    drawRect(screenBuffer, screenWidth, screenHeight, panelX, panelY, panelW, panelH, Engine::makeColor(14, 20, 28));
    drawBorder(screenBuffer, screenWidth, screenHeight, panelX, panelY, panelW, panelH, Engine::makeColor(50, 70, 95));

    int textX = panelX + 5;
    int textY = panelY + 6;
    int maxTextW = panelW - 10;

    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, "PLANTA BAIXA", Engine::makeColor(80, 230, 80));
    textY += 8;
    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, "CHERNOGORSK 64X64", Engine::makeColor(230, 235, 240));
    textY += 11;

    // Setor ativo com quebra de linha automática para não cortar na borda da janela
    const char* sector = map.getSectorName(player.getCamera().pos.x, player.getCamera().pos.y);
    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, "SETOR ATIVO:", Engine::makeColor(140, 150, 165));
    textY += 7;
    textY = drawWrappedText(screenBuffer, screenWidth, screenHeight, textX, textY, maxTextW, sector, Engine::makeColor(245, 215, 60));
    textY += 3;

    // Coordenadas GPS na malha 64x64
    std::string gpsText = "GPS: " + std::to_string(static_cast<int>(player.getCamera().pos.x)) + "," + std::to_string(static_cast<int>(player.getCamera().pos.y));
    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, gpsText, Engine::makeColor(120, 200, 240));
    textY += 9;

    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, "BUSSOLA: [N ^]", Engine::makeColor(220, 80, 80));
    textY += 11;

    // Legenda Compacta
    drawText(screenBuffer, screenWidth, screenHeight, textX, textY, "LEGENDA:", Engine::makeColor(170, 175, 185));
    textY += 8;

    auto drawLegendItem = [&](int texId, uint8_t tr, uint8_t tg, uint8_t tb, const std::string& label, uint32_t borderCol = 0) {
        if (textY + 7 > panelY + panelH - 12) return;
        for (int ly = 0; ly < 5; ++ly) {
            for (int lx = 0; lx < 5; ++lx) {
                uint32_t c = textures.getPixel(texId, lx * 12, ly * 12);
                c = blendTacticalTint(c, tr, tg, tb, 130);
                int sx = textX + lx;
                int sy = textY + ly;
                if (sx < screenWidth && sy < screenHeight) screenBuffer[sy * screenWidth + sx] = c;
            }
        }
        if (borderCol != 0) {
            drawBorder(screenBuffer, screenWidth, screenHeight, textX - 1, textY - 1, 7, 7, borderCol);
        }
        drawText(screenBuffer, screenWidth, screenHeight, textX + 8, textY, label, Engine::makeColor(200, 205, 215));
        textY += 7;
    };

    drawLegendItem(Engine::TextureManager::BRICK_DARK,    155, 95,  78,  "BLOCO TIJOLO");
    drawLegendItem(Engine::TextureManager::CONCRETE,      135, 142, 150, "CONCRETO");
    drawLegendItem(Engine::TextureManager::POLICE_EMBLEM, 70,  130, 210, "SEDE POLICIA");
    drawLegendItem(Engine::TextureManager::HOSPITAL_WALL, 215, 220, 225, "HOSPITAL");
    drawLegendItem(Engine::TextureManager::HANGAR_BLUE,   88,  128, 172, "GALPAO AZUL");
    drawLegendItem(Engine::TextureManager::MURO_SOVIET,   108, 116, 126, "MURO PO-2");
    drawLegendItem(Engine::TextureManager::WOOD_DOOR,     220, 165, 55,  "PORTAS [F]", Engine::makeColor(255, 220, 40));
    drawLegendItem(Engine::TextureManager::CONTAINER_RED, 185, 60,  50,  "CONTAINERS");
    drawLegendItem(Engine::TextureManager::RAILROAD,      95,  88,  82,  "FERROVIA");
    drawLegendItem(Engine::TextureManager::SAND,          215, 192, 135, "PRAIA / CAIS");

    if (textY + 7 <= panelY + panelH - 12) {
        drawRect(screenBuffer, screenWidth, screenHeight, textX, textY, 5, 5, beaconColor);
        drawText(screenBuffer, screenWidth, screenHeight, textX + 8, textY, "SUA POSICAO", Engine::makeColor(255, 235, 70));
    }

    drawText(screenBuffer, screenWidth, screenHeight, textX, panelY + panelH - 8, "[M]: VOLTAR", Engine::makeColor(120, 220, 120));
}

} // namespace DayDoom::Game::UI
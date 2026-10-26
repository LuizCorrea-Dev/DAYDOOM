#include "Raycaster.hpp"
#include "MathDef.hpp"
#include "../game/ChernogorskDef.hpp"
#include <cmath>
#include <algorithm>

namespace DayDoom::Engine {

Raycaster::Raycaster(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight) {
    m_zBuffer.resize(screenWidth, 1000.0f);
}

void Raycaster::render(
    uint32_t* screenBuffer,
    const Camera& camera,
    const Game::CityMap& map,
    const TextureManager& textures
) {
    renderFloorAndCeiling(screenBuffer, camera, map, textures);
    renderWalls(screenBuffer, camera, map, textures);
}

void Raycaster::renderFloorAndCeiling(
    uint32_t* screenBuffer,
    const Camera& camera,
    const Game::CityMap& map,
    const TextureManager& textures
) {
    int halfHeight = m_screenHeight / 2;

    // 1. Renderiza o Céu Apocalíptico de Chernogorsk (Metade superior da tela) com névoa
    for (int y = 0; y < halfHeight; ++y) {
        float skyFactor = static_cast<float>(y) / static_cast<float>(halfHeight);
        uint8_t r = static_cast<uint8_t>(20 + skyFactor * 32);
        uint8_t g = static_cast<uint8_t>(24 + skyFactor * 36);
        uint8_t b = static_cast<uint8_t>(30 + skyFactor * 42);
        uint32_t skyColor = makeColor(r, g, b);

        for (int x = 0; x < m_screenWidth; ++x) {
            screenBuffer[y * m_screenWidth + x] = skyColor;
        }
    }

    // 2. Renderiza o Piso de Chernogorsk (Metade inferior da tela) por projeção de perspectiva
    for (int y = halfHeight; y < m_screenHeight; ++y) {
        float p = static_cast<float>(y - halfHeight);
        if (p < 0.0001f) p = 0.0001f;

        float posZ = 0.5f * static_cast<float>(m_screenHeight);
        float rowDistance = posZ / p;

        float floorStepX = rowDistance * (camera.plane.x * 2.0f) / static_cast<float>(m_screenWidth);
        float floorStepY = rowDistance * (camera.plane.y * 2.0f) / static_cast<float>(m_screenWidth);

        float floorX = camera.pos.x + rowDistance * (camera.dir.x - camera.plane.x);
        float floorY = camera.pos.y + rowDistance * (camera.dir.y - camera.plane.y);

        for (int x = 0; x < m_screenWidth; ++x) {
            int cellX = static_cast<int>(floorX);
            int cellY = static_cast<int>(floorY);

            int tx = static_cast<int>(TextureManager::TEX_SIZE * (floorX - cellX)) & (TextureManager::TEX_SIZE - 1);
            int ty = static_cast<int>(TextureManager::TEX_SIZE * (floorY - cellY)) & (TextureManager::TEX_SIZE - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            int floorType = map.getFloorType(cellX, cellY);
            int texID = TextureManager::ASPHALT;
            switch (floorType) {
                case Game::FLOOR_SAND:      texID = TextureManager::SAND; break;
                case Game::FLOOR_WATER:     texID = TextureManager::WATER; break;
                case Game::FLOOR_CONCRETE:  texID = TextureManager::CONC_FLOOR; break;
                case Game::FLOOR_WOOD:      texID = TextureManager::WOOD_FLOOR; break;
                case Game::FLOOR_RAILROAD:  texID = TextureManager::RAILROAD; break;
                case Game::FLOOR_ASPHALT:
                default:                    texID = TextureManager::ASPHALT; break;
            }

            uint32_t color = textures.getPixel(texID, tx, ty);
            color = applyFog(color, rowDistance, 38.0f);

            screenBuffer[y * m_screenWidth + x] = color;
        }
    }
}

void Raycaster::renderWalls(
    uint32_t* screenBuffer,
    const Camera& camera,
    const Game::CityMap& map,
    const TextureManager& textures
) {
    for (int x = 0; x < m_screenWidth; ++x) {
        float cameraX = 2.0f * static_cast<float>(x) / static_cast<float>(m_screenWidth) - 1.0f;

        float rayDirX = camera.dir.x + camera.plane.x * cameraX;
        float rayDirY = camera.dir.y + camera.plane.y * cameraX;

        int mapX = static_cast<int>(camera.pos.x);
        int mapY = static_cast<int>(camera.pos.y);

        float deltaDistX = (rayDirX == 0.0f) ? 1e30f : std::abs(1.0f / rayDirX);
        float deltaDistY = (rayDirY == 0.0f) ? 1e30f : std::abs(1.0f / rayDirY);

        float sideDistX = 0.0f;
        float sideDistY = 0.0f;

        int stepX = 0;
        int stepY = 0;

        int hit = 0;
        int side = 0;
        float perpWallDist = 0.0f;
        float wallX = 0.0f;
        int hitTexID = TextureManager::BRICK_DARK;

        if (rayDirX < 0.0f) {
            stepX = -1;
            sideDistX = (camera.pos.x - static_cast<float>(mapX)) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (static_cast<float>(mapX) + 1.0f - camera.pos.x) * deltaDistX;
        }

        if (rayDirY < 0.0f) {
            stepY = -1;
            sideDistY = (camera.pos.y - static_cast<float>(mapY)) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (static_cast<float>(mapY) + 1.0f - camera.pos.y) * deltaDistY;
        }

        // Executa o algoritmo DDA com suporte a paredes/muros finos (1/10) e vãos de porta
        int loopCount = 0;
        while (hit == 0 && loopCount++ < 120) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= Game::CityMap::MAP_WIDTH || mapY < 0 || mapY >= Game::CityMap::MAP_HEIGHT) {
                break;
            }

            int cell = map.getCell(mapX, mapY);
            if (cell == 0) continue;

            if (Game::isDoorType(cell)) {
                // Vão de porta: abertura na parede com batentes/marcos laterais fixos
                bool isVert = map.isDoorVertical(mapX, mapY);
                if (isVert) {
                    float distX = (static_cast<float>(mapX) + 0.5f - camera.pos.x) / rayDirX;
                    float hitCoordY = camera.pos.y + distX * rayDirY;
                    float offset = hitCoordY - static_cast<float>(mapY);
                    if (distX > 0.05f) {
                        if (offset >= 0.0f && offset <= 0.06f) {
                            hit = cell;
                            perpWallDist = distX;
                            wallX = offset / 0.06f;
                            side = 0;
                            hitTexID = TextureManager::DOOR_FRAME;
                            break;
                        } else if (offset >= 0.94f && offset <= 1.0f) {
                            hit = cell;
                            perpWallDist = distX;
                            wallX = (offset - 0.94f) / 0.06f;
                            side = 0;
                            hitTexID = TextureManager::DOOR_FRAME;
                            break;
                        }
                        // Raio atravessa o vão central da porta livremente
                        continue;
                    }
                } else {
                    float distY = (static_cast<float>(mapY) + 0.5f - camera.pos.y) / rayDirY;
                    float hitCoordX = camera.pos.x + distY * rayDirX;
                    float offset = hitCoordX - static_cast<float>(mapX);
                    if (distY > 0.05f) {
                        if (offset >= 0.0f && offset <= 0.06f) {
                            hit = cell;
                            perpWallDist = distY;
                            wallX = offset / 0.06f;
                            side = 1;
                            hitTexID = TextureManager::DOOR_FRAME;
                            break;
                        } else if (offset >= 0.94f && offset <= 1.0f) {
                            hit = cell;
                            perpWallDist = distY;
                            wallX = (offset - 0.94f) / 0.06f;
                            side = 1;
                            hitTexID = TextureManager::DOOR_FRAME;
                            break;
                        }
                        continue;
                    }
                }
            } else if (cell == Game::TYPE_CONTAINER_RED || cell == Game::TYPE_CONTAINER_BLU || cell == Game::TYPE_WATER_BARRIER) {
                // Blocos sólidos integrais (Contêineres de carga e barreira de água)
                hit = cell;
                perpWallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
                if (side == 0) {
                    wallX = camera.pos.y + perpWallDist * rayDirY;
                } else {
                    wallX = camera.pos.x + perpWallDist * rayDirX;
                }
                wallX -= std::floor(wallX);
                hitTexID = hit;
                break;
            } else {
                // Paredes de edifícios e muros finos com espessura exata de 1/10 (0.10f)
                bool wallN = (mapY > 0 && map.getCell(mapX, mapY - 1) != 0);
                bool wallS = (mapY < Game::CityMap::MAP_HEIGHT - 1 && map.getCell(mapX, mapY + 1) != 0);
                bool wallW = (mapX > 0 && map.getCell(mapX - 1, mapY) != 0);
                bool wallE = (mapX < Game::CityMap::MAP_WIDTH - 1 && map.getCell(mapX + 1, mapY) != 0);

                bool isHoriz = (wallW || wallE) && !(wallN && wallS);
                bool isVert  = (wallN || wallS);

                if (isHoriz) {
                    // Muro/parede horizontal: face frontal em Y = mapY + 0.5 - 0.05 * stepY
                    float targetY = static_cast<float>(mapY) + 0.5f - 0.05f * static_cast<float>(stepY);
                    float distY = (targetY - camera.pos.y) / rayDirY;
                    float hitCoordX = camera.pos.x + distY * rayDirX;
                    float offset = hitCoordX - static_cast<float>(mapX);

                    if (distY > 0.05f && offset >= 0.0f && offset <= 1.0f) {
                        hit = cell;
                        perpWallDist = distY;
                        wallX = offset;
                        side = 1;
                        hitTexID = cell;
                        break;
                    }
                } else if (isVert) {
                    // Muro/parede vertical: face frontal em X = mapX + 0.5 - 0.05 * stepX
                    float targetX = static_cast<float>(mapX) + 0.5f - 0.05f * static_cast<float>(stepX);
                    float distX = (targetX - camera.pos.x) / rayDirX;
                    float hitCoordY = camera.pos.y + distX * rayDirY;
                    float offset = hitCoordY - static_cast<float>(mapY);

                    if (distX > 0.05f && offset >= 0.0f && offset <= 1.0f) {
                        hit = cell;
                        perpWallDist = distX;
                        wallX = offset;
                        side = 0;
                        hitTexID = cell;
                        break;
                    }
                } else {
                    // Quina ou pilar: testa ambas as faces
                    float targetX = static_cast<float>(mapX) + 0.5f - 0.05f * static_cast<float>(stepX);
                    float distX = (targetX - camera.pos.x) / rayDirX;
                    float hitY = camera.pos.y + distX * rayDirY;

                    float targetY = static_cast<float>(mapY) + 0.5f - 0.05f * static_cast<float>(stepY);
                    float distY = (targetY - camera.pos.y) / rayDirY;
                    float hitX = camera.pos.x + distY * rayDirX;

                    if (distX > 0.05f && (distX < distY || distY <= 0.05f) && hitY >= static_cast<float>(mapY) && hitY <= static_cast<float>(mapY) + 1.0f) {
                        hit = cell;
                        perpWallDist = distX;
                        wallX = hitY - static_cast<float>(mapY);
                        side = 0;
                        hitTexID = cell;
                        break;
                    } else if (distY > 0.05f && hitX >= static_cast<float>(mapX) && hitX <= static_cast<float>(mapX) + 1.0f) {
                        hit = cell;
                        perpWallDist = distY;
                        wallX = hitX - static_cast<float>(mapX);
                        side = 1;
                        hitTexID = cell;
                        break;
                    }
                }
            }
        }

        if (hit == 0) {
            perpWallDist = 50.0f;
        }

        // Teste de colisão da folha da porta girando na dobradiça (espessura 1/20 = 0.05f)
        for (const auto& d : map.getDoors()) {
            float dx = (static_cast<float>(d.x) + 0.5f) - camera.pos.x;
            float dy = (static_cast<float>(d.y) + 0.5f) - camera.pos.y;
            float distCam = dx * camera.dir.x + dy * camera.dir.y;
            if (distCam < -0.5f || (distCam - 1.5f) > perpWallDist) continue;

            float hx = d.hingeX;
            float hy = d.hingeY;
            float theta = d.currentAngle;
            float L = 0.94f;
            float T = Game::Door::THICKNESS; // 0.05f = 1/20
            float h = T * 0.5f;             // 0.025f

            float ux = 0.0f, uy = 0.0f;
            float nx = 0.0f, ny = 0.0f;

            if (d.isVertical) {
                ux = d.swingDir * std::sin(theta);
                uy = std::cos(theta);
                nx = uy;
                ny = -ux;
            } else {
                ux = std::cos(theta);
                uy = d.swingDir * std::sin(theta);
                nx = -uy;
                ny = ux;
            }

            // 4 vértices do contorno da folha da porta
            float v0x = hx + h * nx, v0y = hy + h * ny;
            float v1x = hx + L * ux + h * nx, v1y = hy + L * uy + h * ny;
            float v2x = hx + L * ux - h * nx, v2y = hy + L * uy - h * ny;
            float v3x = hx - h * nx, v3y = hy - h * ny;

            // Face Frontal: V0 -> V1
            {
                float vx = v1x - v0x;
                float vy = v1y - v0y;
                float det = rayDirY * vx - rayDirX * vy;
                if (std::abs(det) > 1e-6f) {
                    float delX = v0x - camera.pos.x;
                    float delY = v0y - camera.pos.y;
                    float t = (delY * vx - delX * vy) / det;
                    float s = (rayDirX * delY - rayDirY * delX) / det;
                    if (t > 0.05f && t < perpWallDist && s >= 0.0f && s <= 1.0f) {
                        perpWallDist = t;
                        hit = d.type;
                        hitTexID = d.type;
                        wallX = s;
                        side = (std::abs(vy) > std::abs(vx)) ? 0 : 1;
                    }
                }
            }

            // Face Traseira: V3 -> V2
            {
                float vx = v2x - v3x;
                float vy = v2y - v3y;
                float det = rayDirY * vx - rayDirX * vy;
                if (std::abs(det) > 1e-6f) {
                    float delX = v3x - camera.pos.x;
                    float delY = v3y - camera.pos.y;
                    float t = (delY * vx - delX * vy) / det;
                    float s = (rayDirX * delY - rayDirY * delX) / det;
                    if (t > 0.05f && t < perpWallDist && s >= 0.0f && s <= 1.0f) {
                        perpWallDist = t;
                        hit = d.type;
                        hitTexID = d.type;
                        wallX = 1.0f - s;
                        side = (std::abs(vy) > std::abs(vx)) ? 0 : 1;
                    }
                }
            }

            // Face da Borda/Ponta (Espessura 1/20): V1 -> V2
            {
                float vx = v2x - v1x;
                float vy = v2y - v1y;
                float det = rayDirY * vx - rayDirX * vy;
                if (std::abs(det) > 1e-6f) {
                    float delX = v1x - camera.pos.x;
                    float delY = v1y - camera.pos.y;
                    float t = (delY * vx - delX * vy) / det;
                    float s = (rayDirX * delY - rayDirY * delX) / det;
                    if (t > 0.05f && t < perpWallDist && s >= 0.0f && s <= 1.0f) {
                        perpWallDist = t;
                        hit = d.type;
                        hitTexID = TextureManager::DOOR_FRAME;
                        wallX = s;
                        side = (std::abs(vy) > std::abs(vx)) ? 0 : 1;
                    }
                }
            }
        }

        if (hit == 0) {
            m_zBuffer[x] = 1000.0f;
            continue;
        }

        if (perpWallDist < 0.01f) perpWallDist = 0.01f;

        m_zBuffer[x] = perpWallDist;

        int lineHeight = static_cast<int>(static_cast<float>(m_screenHeight) / perpWallDist);

        int drawStart = -lineHeight / 2 + m_screenHeight / 2;
        if (drawStart < 0) drawStart = 0;

        int drawEnd = lineHeight / 2 + m_screenHeight / 2;
        if (drawEnd >= m_screenHeight) drawEnd = m_screenHeight - 1;

        wallX -= std::floor(wallX);
        if (wallX < 0.0f) wallX += 1.0f;

        int texX = static_cast<int>(wallX * static_cast<float>(TextureManager::TEX_SIZE));
        if (side == 0 && rayDirX > 0.0f) texX = TextureManager::TEX_SIZE - texX - 1;
        if (side == 1 && rayDirY < 0.0f) texX = TextureManager::TEX_SIZE - texX - 1;
        texX = std::clamp(texX, 0, TextureManager::TEX_SIZE - 1);

        // Mapeia o elemento urbano de Chernogorsk para a textura correspondente
        int texID = TextureManager::BRICK_DARK;
        if (hitTexID == TextureManager::DOOR_FRAME) {
            texID = TextureManager::DOOR_FRAME;
        } else {
            switch (hit) {
                case Game::TYPE_WALL_BRICK:     texID = TextureManager::BRICK_DARK; break;
                case Game::TYPE_WALL_CONCRETE:  texID = TextureManager::CONCRETE; break;
                case Game::TYPE_WALL_METAL:     texID = TextureManager::RUSTY_FENCE; break;
                case Game::TYPE_CONTAINER_RED:  texID = TextureManager::CONTAINER_RED; break;
                case Game::TYPE_CONTAINER_BLU:  texID = TextureManager::CONTAINER_BLUE; break;
                case Game::TYPE_DOOR_WOOD:      texID = TextureManager::WOOD_DOOR; break;
                case Game::TYPE_DOOR_SLIDING:   texID = TextureManager::DOOR_SLIDING; break;
                case Game::TYPE_DOOR_HOSPITAL:  texID = TextureManager::DOOR_HOSPITAL; break;
                case Game::TYPE_GATE_LARGE:     texID = TextureManager::GATE_IRON; break;
                case Game::TYPE_FENCE_WIRE:     texID = TextureManager::FENCE_WIRE; break;
                case Game::TYPE_MURO_SOVIET:    texID = TextureManager::MURO_SOVIET; break;
                case Game::TYPE_MURO_STONE:     texID = TextureManager::MURO_STONE; break;
                case Game::TYPE_WINDOW_GLASS:   texID = TextureManager::STORE_FRONT; break;
                case Game::TYPE_WALL_HOSPITAL:  texID = TextureManager::HOSPITAL_WALL; break;
                case Game::TYPE_HANGAR_BLUE:    texID = TextureManager::HANGAR_BLUE; break;
                case Game::TYPE_POLICE_EMBLEM:  texID = TextureManager::POLICE_EMBLEM; break;
                case Game::TYPE_WATER_BARRIER:  texID = TextureManager::WATER; break;
                default:                        texID = TextureManager::BRICK_DARK; break;
            }
        }

        float step = 1.0f * static_cast<float>(TextureManager::TEX_SIZE) / static_cast<float>(lineHeight);
        float texPos = (static_cast<float>(drawStart) - static_cast<float>(m_screenHeight) / 2.0f + static_cast<float>(lineHeight) / 2.0f) * step;

        for (int y = drawStart; y <= drawEnd; ++y) {
            int texY = static_cast<int>(texPos) & (TextureManager::TEX_SIZE - 1);
            texPos += step;

            uint32_t color = textures.getPixel(texID, texX, texY);
            color = applySideShade(color, side);
            color = applyFog(color, perpWallDist, 38.0f);

            screenBuffer[y * m_screenWidth + x] = color;
        }
    }
}

} // namespace DayDoom::Engine

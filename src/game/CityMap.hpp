#pragma once

#include <vector>
#include <cstdint>
#include "../engine/MathDef.hpp"
#include "ChernogorskDef.hpp"

namespace DayDoom::Game {

class CityMap {
public:
    static constexpr int MAP_WIDTH = 64;
    static constexpr int MAP_HEIGHT = 64;

    CityMap();

    // Acesso aos blocos do mapa
    int getCell(int x, int y) const;
    int getFloorType(int x, int y) const;
    bool isWalkable(float x, float y, float radius = 0.28f) const;

    int getWidth() const { return MAP_WIDTH; }
    int getHeight() const { return MAP_HEIGHT; }

    // Portas, muros e portões interativos
    bool isDoor(int x, int y) const;
    bool isMuro(int x, int y) const;
    void openDoor(int x, int y);

    // Identificação dos 5 Setores de Chernogorsk (Escala 64x64)
    int getSectorID(float x, float y) const;
    const char* getSectorName(float x, float y) const;

    // Ponto de Spawn oficial (Praia a Oeste do Cais Portuário, olhando para o Norte)
    Engine::Vec2 getSpawnPosition() const { return { 12.5f, 54.5f }; }
    float getSpawnAngle() const { return -1.5707963f; } // Olhando para o Norte (-90 graus)

private:
    int m_grid[MAP_WIDTH * MAP_HEIGHT] = {0};
    int m_floorGrid[MAP_WIDTH * MAP_HEIGHT] = {0};

    void buildChernogorskLayout();

    // Helpers de desenho geométrico na grelha 64x64
    void fillRect(int x0, int y0, int x1, int y1, int wallType, int floorType);
    void buildHollowBuilding(int x0, int y0, int x1, int y1, int wallType, int doorX, int doorY, int doorType = TYPE_DOOR_WOOD);
    void buildUShapeBlock(int x0, int y0, int x1, int y1, int wallType, int openSide);
};

} // namespace DayDoom::Game
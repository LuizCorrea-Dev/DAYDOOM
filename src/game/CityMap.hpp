#pragma once

#include <vector>
#include <cstdint>
#include "../engine/MathDef.hpp"
#include "ChernogorskDef.hpp"

namespace DayDoom::Game {

enum class DoorState {
    CLOSED = 0,
    OPENING = 1,
    OPEN = 2,
    CLOSING = 3
};

struct Door {
    int x = 0;
    int y = 0;
    int type = TYPE_DOOR_WOOD;
    DoorState state = DoorState::CLOSED;
    float openProgress = 0.0f; // 0.0f = fechada, 1.0f = totalmente aberta
    bool isVertical = false;   // true = folha no eixo Y (posicionada em X + 0.5), false = folha no eixo X (posicionada em Y + 0.5)
};

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

    // Atualização contínua das animações de portas (deslizamento suave)
    void update(float deltaTime);

    // Sistema completo de portas e portões interagíveis
    bool isDoor(int x, int y) const;
    bool isMuro(int x, int y) const;
    bool isDoorOpen(int x, int y) const;
    float getDoorProgress(int x, int y) const;
    bool isDoorVertical(int x, int y) const;
    int getDoorType(int x, int y) const;

    // Alterna abertura e fechamento de portas com 'F' / 'Espaço'
    void toggleDoor(int x, int y);

    // Identificação dos 5 Setores de Chernogorsk (Escala 64x64)
    int getSectorID(float x, float y) const;
    const char* getSectorName(float x, float y) const;

    // Ponto de Spawn oficial (Praia a Oeste do Cais Portuário, olhando para o Norte)
    Engine::Vec2 getSpawnPosition() const { return { 12.5f, 54.5f }; }
    float getSpawnAngle() const { return -1.5707963f; } // Olhando para o Norte (-90 graus)

    const std::vector<Door>& getDoors() const { return m_doors; }

private:
    int m_grid[MAP_WIDTH * MAP_HEIGHT] = {0};
    int m_floorGrid[MAP_WIDTH * MAP_HEIGHT] = {0};
    std::vector<Door> m_doors;

    void registerDoor(int x, int y, int type);
    Door* findDoor(int x, int y);
    const Door* findDoor(int x, int y) const;

    void buildChernogorskLayout();

    // Helpers de desenho geométrico na grelha 64x64
    void fillRect(int x0, int y0, int x1, int y1, int wallType, int floorType);
    void buildHollowBuilding(int x0, int y0, int x1, int y1, int wallType, int doorX, int doorY, int doorType = TYPE_DOOR_WOOD);
    void buildUShapeBlock(int x0, int y0, int x1, int y1, int wallType, int openSide);
};

} // namespace DayDoom::Game
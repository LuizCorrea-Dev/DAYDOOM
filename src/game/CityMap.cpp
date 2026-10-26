#include "CityMap.hpp"
#include <cmath>
#include <iostream>

namespace DayDoom::Game {

CityMap::CityMap() {
    buildChernogorskLayout();
}

void CityMap::fillRect(int x0, int y0, int x1, int y1, int wallType, int floorType) {
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                m_grid[y * MAP_WIDTH + x] = wallType;
                if (floorType >= 0) {
                    m_floorGrid[y * MAP_WIDTH + x] = floorType;
                }
            }
        }
    }
}

void CityMap::buildHollowBuilding(int x0, int y0, int x1, int y1, int wallType, int doorX, int doorY, int doorType) {
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) continue;
            bool isBorder = (x == x0 || x == x1 || y == y0 || y == y1);
            if (isBorder) {
                m_grid[y * MAP_WIDTH + x] = wallType;
                m_floorGrid[y * MAP_WIDTH + x] = FLOOR_CONCRETE;
            } else {
                m_grid[y * MAP_WIDTH + x] = TYPE_EMPTY;
                m_floorGrid[y * MAP_WIDTH + x] = FLOOR_WOOD;
            }
        }
    }
    if (doorX >= 0 && doorX < MAP_WIDTH && doorY >= 0 && doorY < MAP_HEIGHT) {
        m_grid[doorY * MAP_WIDTH + doorX] = doorType;
    }
}

// openSide: 0=Norte, 1=Sul, 2=Oeste, 3=Leste
void CityMap::buildUShapeBlock(int x0, int y0, int x1, int y1, int wallType, int openSide) {
    // Base do quarteirão como calçada/pátio de concreto
    fillRect(x0, y0, x1, y1, TYPE_EMPTY, FLOOR_CONCRETE);

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            bool n = (y == y0), s = (y == y1), w = (x == x0), e = (x == x1);
            if ((n && openSide != 0) || (s && openSide != 1) ||
                (w && openSide != 2) || (e && openSide != 3)) {
                m_grid[y * MAP_WIDTH + x] = wallType;
            }
        }
    }
    // Coloca uma porta voltada para o pátio interno e janelas nas pontas
    int midX = (x0 + x1) / 2;
    int midY = (y0 + y1) / 2;
    if (openSide == 1 && y0 < MAP_HEIGHT) m_grid[y0 * MAP_WIDTH + midX] = TYPE_DOOR_WOOD;
    if (openSide == 0 && y1 >= 0)         m_grid[y1 * MAP_WIDTH + midX] = TYPE_DOOR_WOOD;
    if (openSide == 3 && x0 < MAP_WIDTH)  m_grid[midY * MAP_WIDTH + x0] = TYPE_DOOR_WOOD;
    if (openSide == 2 && x1 >= 0)         m_grid[midY * MAP_WIDTH + x1] = TYPE_DOOR_WOOD;
}

void CityMap::buildChernogorskLayout() {
    // =========================================================================
    // 1. INICIALIZAÇÃO DA BASE: ASFALTO URBANO (Y: 0..46)
    // =========================================================================
    fillRect(0, 0, 63, 63, TYPE_EMPTY, FLOOR_ASPHALT);

    // =========================================================================
    // 2. ORLA MARÍTIMA SUL, PRAIAS LATERAIS E CAIS CENTRAL (Y: 46..63)
    // (Segue exatamente o recorte da imagem: praias nos lados, doca no centro)
    // =========================================================================
    // Oceano profundo na base (Y: 56..63)
    fillRect(0, 56, 63, 63, TYPE_EMPTY, FLOOR_WATER);

    // Praias curvas laterais (Oeste X: 0..19 e Leste X: 44..63)
    fillRect(0,  49, 19, 55, TYPE_EMPTY, FLOOR_SAND);
    fillRect(44, 49, 63, 55, TYPE_EMPTY, FLOOR_SAND);
    // Água encostando nas praias laterais mais acima (Y: 53..55 nas pontas)
    fillRect(0,  53, 8,  55, TYPE_EMPTY, FLOOR_WATER);
    fillRect(55, 53, 63, 55, TYPE_EMPTY, FLOOR_WATER);

    // Píer de concreto isolado à esquerda do porto (X: 14..15, Y: 49..56)
    fillRect(14, 49, 15, 56, TYPE_EMPTY, FLOOR_CONCRETE);
    m_grid[50 * MAP_WIDTH + 14] = TYPE_WALL_CONCRETE;
    m_grid[50 * MAP_WIDTH + 15] = TYPE_WALL_CONCRETE;

    // GRANDE CAIS PORTUÁRIO CENTRAL (X: 20..43, Y: 47..59)
    fillRect(20, 47, 43, 58, TYPE_EMPTY, FLOOR_CONCRETE);
    // Recorte da bacia do navio cargueiro no cais (X: 33..37, Y: 55..58)
    fillRect(33, 55, 37, 58, TYPE_EMPTY, FLOOR_WATER);

    // Muro perimetral e portões de acesso ao Porto (Y = 47)
    for (int x = 20; x <= 43; ++x) {
        m_grid[47 * MAP_WIDTH + x] = TYPE_MURO_SOVIET;
    }
    m_grid[47 * MAP_WIDTH + 25] = TYPE_GATE_LARGE;
    m_grid[47 * MAP_WIDTH + 26] = TYPE_GATE_LARGE;
    m_grid[47 * MAP_WIDTH + 37] = TYPE_GATE_LARGE;
    m_grid[47 * MAP_WIDTH + 38] = TYPE_GATE_LARGE;

    // Contentores e estruturas dentro do Cais Portuário (Fiel à imagem)
    // Fileiras de contentores à esquerda do porto
    fillRect(21, 48, 22, 50, TYPE_CONTAINER_RED, FLOOR_CONCRETE);
    fillRect(24, 48, 25, 51, TYPE_CONTAINER_BLU, FLOOR_CONCRETE);
    fillRect(22, 52, 23, 55, TYPE_CONTAINER_RED, FLOOR_CONCRETE);
    // Armazém central da doca e guindastes
    buildHollowBuilding(28, 48, 32, 51, TYPE_WALL_METAL, 30, 51, TYPE_DOOR_SLIDING);
    m_grid[55 * MAP_WIDTH + 28] = TYPE_WALL_METAL; // Base Grua 1
    m_grid[55 * MAP_WIDTH + 39] = TYPE_WALL_METAL; // Base Grua 2
    // Contentores à direita do porto + Navio atracado em Y=58
    fillRect(40, 48, 41, 52, TYPE_CONTAINER_BLU, FLOOR_CONCRETE);
    fillRect(38, 58, 42, 59, TYPE_CONTAINER_RED, FLOOR_CONCRETE); // Casco/Carga do Navio

    // Barreira oceânica no limite inferior do mapa (Y = 63)
    for (int x = 0; x < MAP_WIDTH; ++x) {
        m_grid[63 * MAP_WIDTH + x] = TYPE_WATER_BARRIER;
    }

    // =========================================================================
    // 3. EIXO FERROVIÁRIO ESTREITO E CONTÍNUO (Y = 20)
    // =========================================================================
    for (int x = 0; x < MAP_WIDTH; ++x) {
        m_floorGrid[20 * MAP_WIDTH + x] = FLOOR_RAILROAD;
        m_grid[20 * MAP_WIDTH + x] = TYPE_EMPTY;
    }
    // Composição de vagões espaçados na ferrovia
    m_grid[20 * MAP_WIDTH + 10] = TYPE_CONTAINER_RED;
    m_grid[20 * MAP_WIDTH + 11] = TYPE_CONTAINER_RED;
    m_grid[20 * MAP_WIDTH + 32] = TYPE_CONTAINER_BLU;
    m_grid[20 * MAP_WIDTH + 50] = TYPE_WALL_METAL;

    // =========================================================================
    // 4. DEFINIÇÃO DAS 5 FAIXAS HORIZONTAIS DE QUARTEIRÕES E 7 COLUNAS
    // Ruas Verticais (1 a 2 células livres): X = 5, 11, 18, 24, 37, 45, 55, 62
    // Ruas Horizontais: Y = 3, 10, 19..21 (Ferrovia), 28, 36, 45..46
    // =========================================================================

    // --- FAIXA 1 (EXTREMO NORTE: Y = 4..9) ---
    // Col 1 (X: 6..10) & Col 2 (X: 12..17): Casas norte
    buildUShapeBlock(6,  4, 10, 9, TYPE_WALL_BRICK, 0);
    buildUShapeBlock(12, 4, 17, 9, TYPE_WALL_BRICK, 1);
    // Col 3 (X: 19..23): Bloco em L norte
    buildUShapeBlock(19, 4, 23, 9, TYPE_WALL_BRICK, 2);
    // Col 4 (X: 25..36): Comércio Norte (2 prédios médios)
    buildHollowBuilding(25, 5, 29, 9, TYPE_WALL_CONCRETE, 27, 9, TYPE_DOOR_WOOD);
    buildHollowBuilding(31, 5, 36, 9, TYPE_WALL_BRICK,    33, 9, TYPE_DOOR_WOOD);
    // Col 5 (X: 38..44): HOSPITAL CENTRAL DE CHERNOGORSK (Prédio com cruz vermelha)
    buildHollowBuilding(38, 4, 43, 9, TYPE_WALL_HOSPITAL, 40, 9, TYPE_DOOR_HOSPITAL);
    m_grid[9 * MAP_WIDTH + 39] = TYPE_WINDOW_GLASS;
    m_grid[9 * MAP_WIDTH + 41] = TYPE_WINDOW_GLASS;
    // Col 6 (X: 46..54): Posto / Prédio comercial leste
    buildHollowBuilding(47, 6, 52, 9, TYPE_WALL_BRICK, 49, 9, TYPE_DOOR_WOOD);

    // --- FAIXA 2 (ACIMA DA FERROVIA: Y = 11..18) ---
    // Quarteirões residenciais em U/C (Cols 1, 2, 3 e 4)
    buildUShapeBlock(6,  11, 10, 18, TYPE_WALL_BRICK, 3);
    buildUShapeBlock(12, 11, 17, 18, TYPE_WALL_BRICK, 1);
    buildUShapeBlock(19, 11, 23, 18, TYPE_WALL_BRICK, 3);
    buildUShapeBlock(25, 11, 29, 18, TYPE_WALL_BRICK, 3);
    // Galpão Azul médio na Col 4 direita (X: 32..36)
    buildHollowBuilding(32, 11, 36, 18, TYPE_HANGAR_BLUE, 34, 18, TYPE_DOOR_SLIDING);
    // Col 5 (X: 38..44): Grande Hangar Azul + Prédio anexo
    buildHollowBuilding(38, 12, 40, 16, TYPE_WALL_CONCRETE, 39, 16, TYPE_DOOR_WOOD);
    buildHollowBuilding(41, 11, 44, 18, TYPE_HANGAR_BLUE,   42, 18, TYPE_DOOR_SLIDING);
    // Col 6 (X: 46..54): 1 Galpão Cinza + 2 Galpões Azuis gêmeos (Fiel à imagem)
    buildHollowBuilding(46, 12, 48, 17, TYPE_WALL_METAL,  47, 17, TYPE_GATE_LARGE);
    buildHollowBuilding(50, 12, 51, 17, TYPE_HANGAR_BLUE, 50, 17, TYPE_DOOR_SLIDING);
    buildHollowBuilding(53, 12, 54, 17, TYPE_HANGAR_BLUE, 53, 17, TYPE_DOOR_SLIDING);

    // --- FAIXA 3 & 4 CENTRO: A GRANDE ESQUADRA DE POLÍCIA (X: 25..36, Y: 22..35) ---
    // Ocupa o coração do mapa abaixo da ferrovia com pátio e salas internas
    fillRect(25, 22, 36, 35, TYPE_EMPTY, FLOOR_CONCRETE);
    // Muro perimetral do complexo policial
    for (int x = 25; x <= 36; ++x) {
        m_grid[22 * MAP_WIDTH + x] = TYPE_MURO_SOVIET;
        m_grid[35 * MAP_WIDTH + x] = TYPE_MURO_SOVIET;
    }
    for (int y = 22; y <= 35; ++y) {
        m_grid[y * MAP_WIDTH + 25] = TYPE_MURO_SOVIET;
        m_grid[y * MAP_WIDTH + 36] = TYPE_MURO_SOVIET;
    }
    // Entrada sul do pátio policial
    m_grid[35 * MAP_WIDTH + 30] = TYPE_EMPTY;
    m_grid[35 * MAP_WIDTH + 31] = TYPE_EMPTY;

    // Edifício Principal da Esquadra (X: 26..35, Y: 23..33) com divisórias internas
    buildHollowBuilding(26, 23, 35, 33, TYPE_WALL_CONCRETE, 30, 33, TYPE_DOOR_SLIDING);
    m_grid[33 * MAP_WIDTH + 31] = TYPE_DOOR_SLIDING; // Porta dupla principal
    m_grid[33 * MAP_WIDTH + 29] = TYPE_POLICE_EMBLEM;
    m_grid[33 * MAP_WIDTH + 32] = TYPE_POLICE_EMBLEM;

    // Salas internas da Delegacia (Armaria no fundo norte, celas nas laterais)
    for (int x = 26; x <= 35; ++x) m_grid[26 * MAP_WIDTH + x] = TYPE_WALL_CONCRETE;
    m_grid[26 * MAP_WIDTH + 30] = TYPE_DOOR_SLIDING; // Porta da Armaria
    m_grid[27 * MAP_WIDTH + 28] = TYPE_WALL_CONCRETE;
    m_grid[28 * MAP_WIDTH + 28] = TYPE_DOOR_WOOD;    // Sala lateral oeste
    m_grid[27 * MAP_WIDTH + 33] = TYPE_WALL_CONCRETE;
    m_grid[28 * MAP_WIDTH + 33] = TYPE_DOOR_WOOD;    // Sala lateral leste

    // --- FAIXA 3 LATERAIS (ABAIXO DA FERROVIA: Y = 22..27) ---
    buildUShapeBlock(6,  22, 10, 27, TYPE_WALL_BRICK, 1);
    buildUShapeBlock(12, 22, 17, 27, TYPE_WALL_BRICK, 3);
    buildUShapeBlock(19, 22, 23, 27, TYPE_WALL_BRICK, 0);
    // Leste da delegacia (Col 5 & Col 6: Quartel de Bombeiros vermelho e anexos)
    buildUShapeBlock(38, 22, 40, 27, TYPE_WALL_BRICK, 3);
    buildHollowBuilding(42, 22, 44, 26, TYPE_WALL_BRICK, 43, 26, TYPE_DOOR_WOOD);
    buildHollowBuilding(46, 22, 50, 27, TYPE_WALL_HOSPITAL, 48, 27, TYPE_GATE_LARGE); // Bombeiros

    // --- FAIXA 4 LATERAIS (Y = 29..35) ---
    buildUShapeBlock(6,  29, 10, 35, TYPE_WALL_BRICK, 0);
    buildUShapeBlock(12, 29, 17, 35, TYPE_WALL_BRICK, 0);
    buildUShapeBlock(19, 29, 23, 35, TYPE_WALL_BRICK, 2);
    // Col 5 (X: 38..44): Prédio industrial cinza
    buildHollowBuilding(38, 30, 41, 35, TYPE_WALL_CONCRETE, 39, 35, TYPE_DOOR_WOOD);
    // Col 6 (X: 46..54): Fileira de 4 Galpões Industriais Azuis estreitos (Fiel à imagem)
    buildHollowBuilding(46, 30, 47, 35, TYPE_HANGAR_BLUE, 46, 35, TYPE_DOOR_SLIDING);
    buildHollowBuilding(48, 30, 49, 35, TYPE_HANGAR_BLUE, 48, 35, TYPE_DOOR_SLIDING);
    buildHollowBuilding(51, 30, 52, 35, TYPE_HANGAR_BLUE, 51, 35, TYPE_DOOR_SLIDING);
    buildHollowBuilding(53, 30, 54, 35, TYPE_HANGAR_BLUE, 53, 35, TYPE_DOOR_SLIDING);

    // --- FAIXA 5 (SUL URBANO ACIMA DO PORTO: Y = 37..44) ---
    buildUShapeBlock(6,  37, 10, 44, TYPE_WALL_BRICK, 3); // Estacionamento/Prédio SO
    buildUShapeBlock(12, 37, 17, 44, TYPE_WALL_BRICK, 1);
    buildUShapeBlock(19, 37, 23, 44, TYPE_WALL_BRICK, 3);
    // Col 4 Sul (X: 25..36, abaixo da delegacia): 2 Blocos comerciais
    buildUShapeBlock(25, 37, 29, 44, TYPE_WALL_CONCRETE, 3);
    buildHollowBuilding(31, 40, 35, 44, TYPE_WALL_BRICK, 33, 44, TYPE_DOOR_WOOD);
    // Col 5 Sul (X: 38..44): Pátio de caixas + Grande Galpão Azul vertical
    m_grid[38 * MAP_WIDTH + 39] = TYPE_CONTAINER_RED;
    m_grid[40 * MAP_WIDTH + 39] = TYPE_CONTAINER_BLU;
    buildHollowBuilding(42, 37, 44, 44, TYPE_HANGAR_BLUE, 43, 44, TYPE_DOOR_SLIDING);
    // Col 6 Sul (X: 46..54): 4 Armazéns horizontais cinzas em pares
    buildHollowBuilding(46, 37, 49, 40, TYPE_WALL_METAL, 47, 40, TYPE_GATE_LARGE);
    buildHollowBuilding(50, 37, 54, 40, TYPE_WALL_METAL, 52, 40, TYPE_GATE_LARGE);
    buildHollowBuilding(46, 41, 49, 44, TYPE_WALL_METAL, 47, 44, TYPE_GATE_LARGE);
    buildHollowBuilding(50, 41, 54, 44, TYPE_WALL_METAL, 52, 44, TYPE_GATE_LARGE);

    // =========================================================================
    // 5. CERCAS PERIMETRAIS E PORTÃO DE SAÍDA NORTE
    // =========================================================================
    for (int x = 4; x <= 56; ++x) {
        m_grid[2 * MAP_WIDTH + x] = TYPE_MURO_SOVIET;
    }
    // Portão Norte de Saída na Avenida Central (X = 24, 37)
    m_grid[2 * MAP_WIDTH + 24] = TYPE_GATE_LARGE;
    m_grid[2 * MAP_WIDTH + 37] = TYPE_GATE_LARGE;

    // Limites laterais da cidade (Oeste X = 4, Leste X = 56)
    for (int y = 2; y <= 46; ++y) {
        if (y != 20) { // Deixa a ferrovia passar
            m_grid[y * MAP_WIDTH + 4]  = TYPE_FENCE_WIRE;
            m_grid[y * MAP_WIDTH + 56] = TYPE_FENCE_WIRE;
        }
    }
}

int CityMap::getCell(int x, int y) const {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return TYPE_WALL_CONCRETE;
    }
    return m_grid[y * MAP_WIDTH + x];
}

int CityMap::getFloorType(int x, int y) const {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return FLOOR_ASPHALT;
    }
    return m_floorGrid[y * MAP_WIDTH + x];
}

bool CityMap::isWalkable(float x, float y, float radius) const {
    int minX = static_cast<int>(x - radius);
    int maxX = static_cast<int>(x + radius);
    int minY = static_cast<int>(y - radius);
    int maxY = static_cast<int>(y + radius);

    for (int cy = minY; cy <= maxY; ++cy) {
        for (int cx = minX; cx <= maxX; ++cx) {
            int cell = getCell(cx, cy);
            if (cell != TYPE_EMPTY) {
                return false;
            }
        }
    }
    return true;
}

bool CityMap::isDoor(int x, int y) const {
    return isDoorType(getCell(x, y));
}

bool CityMap::isMuro(int x, int y) const {
    return isMuroType(getCell(x, y));
}

void CityMap::openDoor(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        int cell = m_grid[y * MAP_WIDTH + x];
        if (isDoorType(cell)) {
            m_grid[y * MAP_WIDTH + x] = TYPE_EMPTY;
            // Se for portão duplo ou porta dupla, abre a célula adjacente também
            if (x + 1 < MAP_WIDTH && isDoorType(m_grid[y * MAP_WIDTH + (x + 1)])) {
                m_grid[y * MAP_WIDTH + (x + 1)] = TYPE_EMPTY;
            }
            if (x - 1 >= 0 && isDoorType(m_grid[y * MAP_WIDTH + (x - 1)])) {
                m_grid[y * MAP_WIDTH + (x - 1)] = TYPE_EMPTY;
            }
        }
    }
}

int CityMap::getSectorID(float x, float y) const {
    if (y >= 47.0f) return 1; // Setor 1: Praia do Mar Báltico & Cais Portuário
    if (y >= 36.0f) return 2; // Setor 2: Área Residencial Sul & Comércio
    if (x >= 25.0f && x <= 36.0f && y >= 22.0f && y <= 35.0f) return 3; // Setor 3: Esquadra Central
    if (y >= 19.0f) return 4; // Setor 4: Ferrovia, Bairros Centrais & Galpões
    return 5;                 // Setor 5: Periferia Norte & Hospital (Saída)
}

const char* CityMap::getSectorName(float x, float y) const {
    int sector = getSectorID(x, y);
    return g_ChernoSectors[sector - 1].name;
}

} // namespace DayDoom::Game
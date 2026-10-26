#pragma once

#include <cstdint>

namespace DayDoom::Game {

// ==============================================================================
// 1. DIMENSÕES OFICIAIS DA MALHA FINA (64x64)
// ==============================================================================

constexpr int MAP_GRID_COLS = 64;
constexpr int MAP_GRID_ROWS = 64;

// ==============================================================================
// 2. ELEMENTOS URBANOS EM ALTA RESOLUÇÃO (PAREDES vs MUROS vs PORTAS)
// ==============================================================================

enum UrbanElementType {
    TYPE_EMPTY = 0,         // Rua / Calçada / Pátio livre
    // --- PAREDES DE EDIFÍCIOS ---
    TYPE_WALL_BRICK = 1,    // Prédios residenciais e comerciais (L, U, C)
    TYPE_WALL_CONCRETE = 2, // Concreto cinza de prédios públicos, esquadra e cais
    TYPE_WALL_METAL = 3,    // Armazéns industriais e galpões cinzas
    TYPE_CONTAINER_RED = 4, // Contêiner marítimo vermelho
    TYPE_CONTAINER_BLU = 5, // Contêiner marítimo azul
    TYPE_WINDOW_GLASS = 10, // Vitrines e janelas
    TYPE_WALL_HOSPITAL = 11,// Hospital de Chernogorsk (fachada branca / cruz vermelha)
    TYPE_WATER_BARRIER = 12,// Limite marítimo no oceano
    TYPE_HANGAR_BLUE = 13,  // Galpões industriais de telhado azul (Leste)
    TYPE_POLICE_EMBLEM = 14,// Fachada da Esquadra de Polícia com brasão

    // --- MUROS DIVISÓRIOS E PERIMETRAIS ---
    TYPE_MURO_SOVIET = 15,  // Muro soviético PO-2 de placas de concreto
    TYPE_MURO_STONE = 16,   // Muro de alvenaria / divisória de pátio
    TYPE_FENCE_WIRE = 9,    // Cerca perimetral de arame

    // --- PORTAS E PORTÕES INTERATIVOS (Abertos com 'F' / 'Espaço') ---
    TYPE_DOOR_WOOD = 6,     // Porta de madeira residencial
    TYPE_DOOR_SLIDING = 7,  // Porta corrediça de aço policial / industrial
    TYPE_DOOR_HOSPITAL = 17,// Porta dupla de emergência do hospital
    TYPE_GATE_LARGE = 8     // Portão de ferro quadriculado e saída norte
};

// Tipos de piso / chão de Chernogorsk
enum FloorType {
    FLOOR_ASPHALT = 0,      // Avenidas e ruas asfaltadas
    FLOOR_SAND = 1,         // Areia da praia do Mar Báltico
    FLOOR_WATER = 2,        // Água do Mar Báltico
    FLOOR_CONCRETE = 3,     // Calçadas, pátios de quarteirões e cais portuário
    FLOOR_WOOD = 4,         // Assoalho interno dos edifícios
    FLOOR_RAILROAD = 5      // Linha férrea horizontal
};

// Funções de categorização rápida
inline bool isDoorType(int type) {
    return (type == TYPE_DOOR_WOOD || type == TYPE_DOOR_SLIDING ||
            type == TYPE_DOOR_HOSPITAL || type == TYPE_GATE_LARGE);
}

inline bool isMuroType(int type) {
    return (type == TYPE_MURO_SOVIET || type == TYPE_MURO_STONE || type == TYPE_FENCE_WIRE);
}

inline bool isWallType(int type) {
    return (type == TYPE_WALL_BRICK || type == TYPE_WALL_CONCRETE || type == TYPE_WALL_METAL ||
            type == TYPE_CONTAINER_RED || type == TYPE_CONTAINER_BLU || type == TYPE_WINDOW_GLASS ||
            type == TYPE_WALL_HOSPITAL || type == TYPE_HANGAR_BLUE || type == TYPE_POLICE_EMBLEM);
}

// Estruturas de Setores para HUD e Automap
struct MapSector {
    int16_t floorheight;
    int16_t ceilingheight;
    const char* floorflat;
    const char* ceilingflat;
    uint16_t lightlevel;
    uint16_t special;
    const char* name;
};

// --- OS 5 SETORES OFICIAIS DE CHERNOGORSK ---
constexpr MapSector g_ChernoSectors[] = {
    { 0,  256, "SAND01",  "F_SKY1", 192, 0,   "SETOR 1: PRAIA DO MAR BALTICO & PORTO" },
    { 16, 160, "DIRT02",  "CEIL01", 160, 0,   "SETOR 2: AREA RESIDENCIAL SUL & COMERCIO" },
    { 16, 144, "CONC01",  "CEIL02", 128, 101, "SETOR 3: ESQUADRA DE POLICIA CENTRAL" },
    { 8,  200, "ASPHALT", "F_SKY1", 200, 0,   "SETOR 4: LINHA FERREA & ZONA INDUSTRIAL" },
    { 0,  224, "TILE01",  "CEIL03", 144, 102, "SETOR 5: PERIFERIA NORTE & HOSPITAL (SAIDA)" }
};

constexpr int16_t TAG_POLICE_ARMORY_DOOR = 101;
constexpr int16_t TAG_PORT_EXIT_GATE     = 102;

} // namespace DayDoom::Game
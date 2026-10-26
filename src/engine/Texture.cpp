#include "Texture.hpp"
#include "MathDef.hpp"
#include <cmath>

namespace DayDoom::Engine {

TextureManager::TextureManager() {
    for (auto& tex : m_textures) {
        tex.resize(TEX_SIZE * TEX_SIZE, 0xFF000000);
    }
    initProceduralTextures();
}

void TextureManager::initProceduralTextures() {
    // 0. BRICK_DARK (BRICK01): Tijolos escuros com argamassa desgastada de prédios
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isMortar = (y % 8 == 0) || ((y / 8) % 2 == 0 && x % 16 == 0) || ((y / 8) % 2 == 1 && (x + 8) % 16 == 0);
            if (isMortar) {
                m_textures[BRICK_DARK][y * TEX_SIZE + x] = makeColor(35, 30, 28);
            } else {
                int noise = (x * 7 + y * 13) % 20;
                m_textures[BRICK_DARK][y * TEX_SIZE + x] = makeColor(110 + noise, 45 + noise / 2, 35 + noise / 2);
            }
        }
    }

    // 1. CONCRETE (CONC02): Concreto cinzento manchado de umidade da Esquadra de Polícia
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int noise = ((x ^ y) * 9 + (x + y) * 5) % 35;
            uint8_t c = 75 + noise;
            if (y > 45 && ((x + y) % 7 == 0)) {
                m_textures[CONCRETE][y * TEX_SIZE + x] = makeColor(40, 65, 35); // Musgo nos cantos
            } else {
                m_textures[CONCRETE][y * TEX_SIZE + x] = makeColor(c, c, c - 5);
            }
        }
    }

    // 2. STORE_FRONT (WINCOMM): Fachada com vitrine quebrada e tábuas de barricada do supermercado
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isFrame = (x < 4 || x > 59 || y < 4 || y > 59);
            bool isBoard = (y >= 20 && y <= 27) || (y >= 36 && y <= 43) || (x + y >= 45 && x + y <= 52);

            if (isFrame) {
                m_textures[STORE_FRONT][y * TEX_SIZE + x] = makeColor(45, 45, 50);
            } else if (isBoard) {
                int grain = (x * 3) % 15;
                m_textures[STORE_FRONT][y * TEX_SIZE + x] = makeColor(130 + grain, 85 + grain, 45 + grain);
            } else {
                int glass = (x % 11 == y % 7) ? 90 : 30;
                m_textures[STORE_FRONT][y * TEX_SIZE + x] = makeColor(25, 35 + glass / 4, 45 + glass / 3);
            }
        }
    }

    // 3. WOOD_DOOR (DOORWOOD): Porta de madeira com ripas e maçaneta de bronze
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isBorder = (x < 3 || x > 60 || y < 3 || y > 60 || x % 16 == 0);
            bool isHandle = (x >= 8 && x <= 12 && y >= 30 && y <= 34);

            if (isHandle) {
                m_textures[WOOD_DOOR][y * TEX_SIZE + x] = makeColor(190, 160, 50);
            } else if (isBorder) {
                m_textures[WOOD_DOOR][y * TEX_SIZE + x] = makeColor(50, 30, 20);
            } else {
                int woodGrain = (y * 5 + x * 2) % 20;
                m_textures[WOOD_DOOR][y * TEX_SIZE + x] = makeColor(100 + woodGrain, 60 + woodGrain, 30 + woodGrain / 2);
            }
        }
    }

    // 4. RUSTY_FENCE (CORRMT1): Chapa metálica ondulada dos armazéns industriais
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int wave = (x % 8 < 4) ? 22 : 0;
            int rust = ((x * 13 + y * 17) % 50 > 30) ? 45 : 0;
            if (rust > 0) {
                m_textures[RUSTY_FENCE][y * TEX_SIZE + x] = makeColor(120 + rust, 50 + rust / 2, 25);
            } else {
                m_textures[RUSTY_FENCE][y * TEX_SIZE + x] = makeColor(70 + wave, 75 + wave, 80 + wave);
            }
        }
    }

    // 5. ASPHALT: Asfalto com rachaduras e faixa amarela gasta
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int grain = (x * 11 + y * 7) % 25;
            bool isYellowStripe = (x >= 30 && x <= 34) && (y % 16 < 10);
            if (isYellowStripe && (grain < 18)) {
                m_textures[ASPHALT][y * TEX_SIZE + x] = makeColor(160, 140, 30);
            } else {
                uint8_t a = 35 + grain;
                m_textures[ASPHALT][y * TEX_SIZE + x] = makeColor(a, a, a);
            }
        }
    }

    // 6. WOOD_FLOOR: Piso de tábuas corridas para o interior das casas
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isPlankBorder = (y % 16 == 0) || ((y / 16) % 2 == 0 && x % 32 == 0);
            if (isPlankBorder) {
                m_textures[WOOD_FLOOR][y * TEX_SIZE + x] = makeColor(30, 20, 10);
            } else {
                int g = (x * 2 + y * 7) % 18;
                m_textures[WOOD_FLOOR][y * TEX_SIZE + x] = makeColor(115 + g, 80 + g, 50 + g);
            }
        }
    }

    // 7. SKY_OVERCAST (F_SKY1): Céu nublado e decadente de apocalipse
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int cloud = ((x * 4 + y * 8) ^ (x * 2)) % 25;
            uint8_t gray = 50 + (y / 2) + cloud;
            m_textures[SKY_OVERCAST][y * TEX_SIZE + x] = makeColor(gray + 5, gray, gray - 5);
        }
    }

    // 8. CONTAINER_RED (CONTRED): Contêiner marítimo industrial vermelho com frisos verticais
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isBorder = (x < 2 || x > 61 || y < 2 || y > 61);
            int ridge = (x % 6 < 3) ? 20 : -15;
            int rust = ((x * 9 + y * 13) % 40 > 32) ? 30 : 0;
            if (isBorder) {
                m_textures[CONTAINER_RED][y * TEX_SIZE + x] = makeColor(40, 20, 15);
            } else if (rust > 0) {
                m_textures[CONTAINER_RED][y * TEX_SIZE + x] = makeColor(100 + rust, 45, 20);
            } else {
                int r = std::clamp(140 + ridge, 40, 230);
                m_textures[CONTAINER_RED][y * TEX_SIZE + x] = makeColor(r, 35, 30);
            }
        }
    }

    // 9. CONTAINER_BLUE (CONTBLU): Contêiner marítimo industrial azul com frisos verticais
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isBorder = (x < 2 || x > 61 || y < 2 || y > 61);
            int ridge = (x % 6 < 3) ? 25 : -15;
            int rust = ((x * 11 + y * 15) % 40 > 32) ? 35 : 0;
            if (isBorder) {
                m_textures[CONTAINER_BLUE][y * TEX_SIZE + x] = makeColor(15, 25, 45);
            } else if (rust > 0) {
                m_textures[CONTAINER_BLUE][y * TEX_SIZE + x] = makeColor(110 + rust, 55, 25);
            } else {
                int b = std::clamp(130 + ridge, 30, 220);
                m_textures[CONTAINER_BLUE][y * TEX_SIZE + x] = makeColor(30, 65 + ridge / 2, b);
            }
        }
    }

    // 10. DOOR_SLIDING (DOORMLT): Porta corrediça de aço reforçado com faixas de alerta da polícia
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isHazard = (y >= 54 && y <= 60) && ((x + y) % 12 < 6);
            bool isLockPanel = (x >= 48 && x <= 56 && y >= 28 && y <= 36);
            bool isFrame = (x < 4 || x > 59 || y < 4 || y > 59);
            bool isSeam = (x == 32);

            if (isLockPanel) {
                // Painel de leitor biométrico/cartão de acesso
                m_textures[DOOR_SLIDING][y * TEX_SIZE + x] = (y == 32 && x >= 51 && x <= 53) ? makeColor(30, 240, 50) : makeColor(20, 20, 25);
            } else if (isHazard) {
                m_textures[DOOR_SLIDING][y * TEX_SIZE + x] = ((x + y) % 12 < 6) ? makeColor(210, 180, 20) : makeColor(25, 25, 25);
            } else if (isFrame || isSeam) {
                m_textures[DOOR_SLIDING][y * TEX_SIZE + x] = makeColor(40, 45, 50);
            } else {
                int steel = (x * 3 + y * 7) % 15;
                m_textures[DOOR_SLIDING][y * TEX_SIZE + x] = makeColor(85 + steel, 90 + steel, 100 + steel);
            }
        }
    }

    // 11. GATE_IRON (GATEIRON): Portão de ferro quadriculado das docas e armazéns
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isFrame = (x < 4 || x > 59 || y < 4 || y > 59);
            bool isBar = (x % 8 <= 1) || (y % 16 <= 1);
            bool isCenterLock = (x >= 28 && x <= 36 && y >= 26 && y <= 38);

            if (isCenterLock) {
                m_textures[GATE_IRON][y * TEX_SIZE + x] = makeColor(140, 110, 30); // Cadeado pesado de bronze
            } else if (isFrame || isBar) {
                int rust = (x * y) % 15;
                m_textures[GATE_IRON][y * TEX_SIZE + x] = makeColor(55 + rust, 55, 60);
            } else {
                m_textures[GATE_IRON][y * TEX_SIZE + x] = makeColor(15, 18, 22); // Espaço vazado escuro
            }
        }
    }

    // 12. FENCE_WIRE (FENCEWI): Cerca de arame farpado perimetral com malha diagonal
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isPost = (x % 32 < 3);
            bool isMesh = ((x + y) % 8 == 0) || ((x - y + 64) % 8 == 0);
            bool isTopBarbed = (y < 6) && ((x % 6) == 0);

            if (isPost) {
                m_textures[FENCE_WIRE][y * TEX_SIZE + x] = makeColor(70, 75, 80);
            } else if (isTopBarbed || isMesh) {
                m_textures[FENCE_WIRE][y * TEX_SIZE + x] = makeColor(140, 145, 155);
            } else {
                m_textures[FENCE_WIRE][y * TEX_SIZE + x] = makeColor(20, 25, 30); // Fundo escuro
            }
        }
    }

    // 13. HOSPITAL_WALL (HOSP01): Fachada do hospital soviético branca com cruz vermelha
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isCross = (x >= 26 && x <= 37 && y >= 16 && y <= 47) || (x >= 16 && x <= 47 && y >= 26 && y <= 37);
            bool isBorder = (x < 2 || x > 61 || y < 2 || y > 61);

            if (isBorder) {
                m_textures[HOSPITAL_WALL][y * TEX_SIZE + x] = makeColor(90, 95, 95);
            } else if (isCross) {
                m_textures[HOSPITAL_WALL][y * TEX_SIZE + x] = makeColor(190, 30, 30);
            } else {
                int plaster = (x * 5 + y * 9) % 18;
                m_textures[HOSPITAL_WALL][y * TEX_SIZE + x] = makeColor(165 + plaster, 170 + plaster, 165 + plaster);
            }
        }
    }

    // 14. SAND (SAND01): Areia da praia do Mar Báltico (grãos e ondulações finas)
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int grain = ((x * 17) ^ (y * 19)) % 25;
            int ripple = static_cast<int>(std::sin((x + y * 0.5f) * 0.4f) * 6.0f);
            uint8_t r = std::clamp(175 + grain + ripple, 0, 255);
            uint8_t g = std::clamp(145 + grain / 2 + ripple, 0, 255);
            uint8_t b = std::clamp(90 + grain / 3, 0, 255);
            m_textures[SAND][y * TEX_SIZE + x] = makeColor(r, g, b);
        }
    }

    // 15. WATER (WATER01): Água azul do Mar Báltico com ondas e reflexos
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int wave = static_cast<int>(std::sin(x * 0.35f + y * 0.2f) * 18.0f);
            int foam = ((x * 13 + y * 7) % 30 > 27) ? 40 : 0;
            uint8_t r = 20 + foam;
            uint8_t g = std::clamp(60 + wave + foam, 15, 200);
            uint8_t b = std::clamp(130 + wave + foam, 50, 240);
            m_textures[WATER][y * TEX_SIZE + x] = makeColor(r, g, b);
        }
    }

    // 16. CONC_FLOOR (CONC01): Chão de concreto polido das docas e interior da delegacia
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isTileBorder = (x % 32 == 0) || (y % 32 == 0);
            if (isTileBorder) {
                m_textures[CONC_FLOOR][y * TEX_SIZE + x] = makeColor(35, 35, 40);
            } else {
                int speck = (x * 7 + y * 11) % 15;
                uint8_t c = 80 + speck;
                m_textures[CONC_FLOOR][y * TEX_SIZE + x] = makeColor(c, c, c + 2);
            }
        }
    }

    // 17. RAILROAD: Trilhos de trem com dormentes de madeira horizontais e trilho de ferro
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isSleeper = (y % 16 >= 4 && y % 16 <= 11); // Dormente de madeira
            bool isRail = (x >= 14 && x <= 18) || (x >= 46 && x <= 50); // Trilho de aço
            bool isGravel = !isSleeper && !isRail;

            if (isRail) {
                int shine = (x == 16 || x == 48) ? 60 : 0;
                m_textures[RAILROAD][y * TEX_SIZE + x] = makeColor(140 + shine, 145 + shine, 155 + shine);
            } else if (isSleeper) {
                int wood = (x * 3 + y * 5) % 15;
                m_textures[RAILROAD][y * TEX_SIZE + x] = makeColor(80 + wood, 50 + wood, 25 + wood / 2);
            } else {
                int rock = ((x * 11) ^ (y * 13)) % 20;
                m_textures[RAILROAD][y * TEX_SIZE + x] = makeColor(50 + rock, 50 + rock, 55 + rock);
            }
        }
    }

    // 18. HANGAR_BLUE (HANGARBL): Galpão industrial com chapa metálica ondulada azul (Docas leste)
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            int wave = (x % 6 < 3) ? 22 : -12;
            int rust = ((x * 7 + y * 13) % 45 > 35) ? 35 : 0;
            bool isRivet = (y % 16 == 0) && (x % 8 == 0);

            if (isRivet) {
                m_textures[HANGAR_BLUE][y * TEX_SIZE + x] = makeColor(180, 190, 205);
            } else if (rust > 0) {
                m_textures[HANGAR_BLUE][y * TEX_SIZE + x] = makeColor(90 + rust, 50, 25);
            } else {
                int b = std::clamp(100 + wave, 40, 210);
                int g = std::clamp(60 + wave / 2, 20, 150);
                m_textures[HANGAR_BLUE][y * TEX_SIZE + x] = makeColor(35, g, b);
            }
        }
    }

    // 19. POLICE_EMBLEM (POLICESH): Fachada de concreto com o grande brasão do escudo da Polícia
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            // Desenha um escudo heráldico estilizado na região central (x: 18..45, y: 12..52)
            bool inShieldX = (x >= 20 && x <= 43);
            int distFromCenter = std::abs(x - 31);
            int shieldBottom = 50 - distFromCenter * 2;
            bool inShield = inShieldX && (y >= 14 && y <= shieldBottom);
            bool isBorder = inShield && (x <= 22 || x >= 41 || y <= 16 || y >= shieldBottom - 2);
            bool isStar = inShield && (std::abs(x - 31) <= 2 && std::abs(y - 28) <= 6) || (std::abs(x - 31) <= 6 && std::abs(y - 28) <= 2);

            if (isBorder) {
                m_textures[POLICE_EMBLEM][y * TEX_SIZE + x] = makeColor(210, 220, 240); // Borda prateada
            } else if (isStar) {
                m_textures[POLICE_EMBLEM][y * TEX_SIZE + x] = makeColor(230, 200, 50);  // Estrela dourada no centro
            } else if (inShield) {
                int blueShade = 90 + (y / 2);
                m_textures[POLICE_EMBLEM][y * TEX_SIZE + x] = makeColor(20, 45, blueShade); // Azul da polícia
            } else {
                // Fundo de concreto cinza
                int noise = ((x ^ y) * 9 + (x + y) * 5) % 25;
                uint8_t c = 75 + noise;
                m_textures[POLICE_EMBLEM][y * TEX_SIZE + x] = makeColor(c, c, c - 5);
            }
        }
    }

    // 20. MURO_SOVIET (MUROPO2): Muro soviético PO-2 de placas de concreto com relevo piramidal em diamante
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isTopCap = (y < 6);
            bool isGroove = (y == 6 || y == 7);
            bool isPillar = (x < 4 || x > 59);

            if (isTopCap) {
                // Topo chanfrado de concreto protetor contra chuva
                int shade = 95 - y * 3;
                m_textures[MURO_SOVIET][y * TEX_SIZE + x] = makeColor(shade, shade, shade - 2);
            } else if (isGroove) {
                // Fresta escura de dilatação
                m_textures[MURO_SOVIET][y * TEX_SIZE + x] = makeColor(30, 30, 32);
            } else if (isPillar) {
                // Pilar lateral de sustentação da placa pré-moldada
                int pNoise = (x * 7 + y * 13) % 15;
                uint8_t pc = 72 + pNoise;
                m_textures[MURO_SOVIET][y * TEX_SIZE + x] = makeColor(pc, pc, pc - 4);
            } else {
                // Painel com relevo de losango clássico do padrão PO-2 ("Plita Ograzhdeniya")
                int lx = (x - 4) % 18;
                int ly = (y - 8) % 18;
                int dx = std::abs(lx - 9);
                int dy = std::abs(ly - 9);
                int dist = dx + dy;

                int baseTone = 80;
                // Iluminação direcional no relevo piramidal (facetas claras a noroeste, escuras a sudeste)
                int facetLight = (lx <= 9 && ly <= 9) ? 18 : ((lx > 9 && ly > 9) ? -18 : 0);
                int tone = std::clamp(baseTone + (9 - dist) * 3 + facetLight, 35, 140);

                if (y > 52 && ((x + y) % 5 == 0)) {
                    // Musgo e umidade no rodapé do muro
                    m_textures[MURO_SOVIET][y * TEX_SIZE + x] = makeColor(40, 58, 35);
                } else {
                    int speck = (x * 11 + y * 17) % 12;
                    m_textures[MURO_SOVIET][y * TEX_SIZE + x] = makeColor(tone + speck, tone + speck, tone + speck - 4);
                }
            }
        }
    }

    // 21. MURO_STONE (MUROSTN): Muro de pedra rústica e alvenaria para quintais e divisórias residenciais
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isTopCap = (y < 5);
            bool isCourseMortar = (y % 14 == 0);
            int courseIndex = y / 14;
            int xOffset = (courseIndex % 2 == 0) ? 0 : 10;
            bool isVertMortar = ((x + xOffset) % 20 == 0);

            if (isTopCap) {
                // Borda de cimento alisado
                m_textures[MURO_STONE][y * TEX_SIZE + x] = makeColor(90, 88, 85);
            } else if (isCourseMortar || isVertMortar) {
                // Argamassa nas juntas entre as pedras
                m_textures[MURO_STONE][y * TEX_SIZE + x] = makeColor(42, 38, 35);
            } else {
                // Textura de pedra rústica de Chernogorsk
                int stoneId = (courseIndex * 7 + (x + xOffset) / 20) % 4;
                int stoneNoise = ((x * 13) ^ (y * 7)) % 22;
                uint8_t r = 90 + stoneNoise;
                uint8_t g = 80 + stoneNoise;
                uint8_t b = 70 + stoneNoise;

                if (stoneId == 1) { r += 15; g += 10; } // Tom arenoso
                else if (stoneId == 2) { r -= 10; g -= 5; b += 8; } // Tom ardósia cinzento
                else if (stoneId == 3) { r += 8; g -= 5; } // Tom avermelhado mineral

                if (y > 54 && ((x + y) % 4 == 0)) {
                    // Manchas de musgo nas frestas inferiores
                    m_textures[MURO_STONE][y * TEX_SIZE + x] = makeColor(45, 60, 32);
                } else {
                    m_textures[MURO_STONE][y * TEX_SIZE + x] = makeColor(r, g, b);
                }
            }
        }
    }

    // 22. DOOR_HOSPITAL (DOORHOSP): Porta dupla branca de hospital com vidros aramados e cruz vermelha
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            bool isFrame = (x < 3 || x > 60 || y < 3 || y > 60);
            bool isCenterSplit = (x == 31 || x == 32);
            bool isPushBar = (y >= 30 && y <= 33);
            bool isGlass1 = (x >= 10 && x <= 23 && y >= 10 && y <= 27);
            bool isGlass2 = (x >= 40 && x <= 53 && y >= 10 && y <= 27);
            bool isRedCross1 = ((x >= 15 && x <= 18 && y >= 40 && y <= 50) || (x >= 11 && x <= 22 && y >= 43 && y <= 47));
            bool isRedCross2 = ((x >= 45 && x <= 48 && y >= 40 && y <= 50) || (x >= 41 && x <= 52 && y >= 43 && y <= 47));

            if (isFrame || isCenterSplit) {
                m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(60, 65, 70); // Moldura metálica escura
            } else if (isGlass1 || isGlass2) {
                // Vidro com malha de proteção aramada
                bool isWire = (x % 4 == 0) || (y % 4 == 0);
                if (isWire) {
                    m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(140, 160, 180);
                } else {
                    m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(40, 65, 80);
                }
            } else if (isPushBar) {
                // Barra de empurrar antipânico em latão dourado
                m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(210, 180, 50);
            } else if (isRedCross1 || isRedCross2) {
                // Emblema da Cruz Vermelha hospitalar
                m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(220, 25, 25);
            } else {
                // Pintura branca esmaltada de hospital soviético
                int whiteNoise = (x * 5 + y * 7) % 15;
                uint8_t w = 185 + whiteNoise;
                m_textures[DOOR_HOSPITAL][y * TEX_SIZE + x] = makeColor(w, w + 2, w + 2);
            }
        }
    }
}

} // namespace DayDoom::Engine


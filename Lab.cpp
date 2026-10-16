#include "Lab.h"
#include "Chunk.h"
#include <algorithm>


float GetHash(int x, int z, unsigned int seed) {
    int n = x + z * 57 + seed * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

CellType Maze::GetCellTypeGlobal(int worldX, int worldZ) const {
    constexpr int ROOM_SIZE = 8;

    //global coords of block
    int gx = (int)glm::floor((float)worldX / ROOM_SIZE);
    int gz = (int)glm::floor((float)worldZ / ROOM_SIZE);

    //local
    int lx = ((worldX % ROOM_SIZE) + ROOM_SIZE) % ROOM_SIZE;
    int lz = ((worldZ % ROOM_SIZE) + ROOM_SIZE) % ROOM_SIZE;

    if (lx == 0 && lz == 0) return CellType::WALL;

    if (lx == 0) {
        // 30% chance of no wall
        if (std::abs(GetHash(gx, gz, globalSeed + 1)) < 0.30f) return CellType::EMPTY;

        // if wall, 70% chance of doorway
   
        if (std::abs(GetHash(gx, gz, globalSeed + 2)) < 0.70f) {
            if (lz == 3 || lz == 4) return CellType::DOORWAY;
        }
        return CellType::WALL;
    }

    if (lz == 0) {
        if (std::abs(GetHash(gx, gz, globalSeed + 3)) < 0.30f) return CellType::EMPTY;

        if (std::abs(GetHash(gx, gz, globalSeed + 4)) < 0.70f) {
            if (lx == 3 || lx == 4) return CellType::DOORWAY;
        }
        return CellType::WALL;
    }

    //added some elements inside rooms for more chaos
    float roomTheme = std::abs(GetHash(gx, gz, globalSeed + 5));

    // 5% column in the center
    if (roomTheme < 0.05f) {
        if (lx >= 3 && lx <= 4 && lz >= 3 && lz <= 4) return CellType::WALL;
    }
   

    else if (roomTheme < 0.15f) {
        if (lx == 4 && lz >= 2 && lz <= 5) return CellType::WALL;
    }

    return CellType::EMPTY;
}

Maze::Maze(Material* wallMat, Material* floorMat, Material* ceilingMat, unsigned int seed)
    : wallMaterial(wallMat), floorMaterial(floorMat), ceilingMaterial(ceilingMat), globalSeed(seed),
    renderDistance(gameConfig.renderDistance) 
{
}

Maze::~Maze() {}

void Maze::Update(const glm::vec3& playerPos, const glm::vec3& cameraFront, float deltaTime) {
    int currentChunkX = (int)glm::floor(playerPos.x / (CHUNK_SIZE * CELL_SIZE));
    int currentChunkZ = (int)glm::floor(playerPos.z / (CHUNK_SIZE * CELL_SIZE));

    for (auto it = activeChunks.begin(); it != activeChunks.end(); ) {
        int cx = it->first.first;
        int cz = it->first.second;
        if (std::abs(cx - currentChunkX) > renderDistance || std::abs(cz - currentChunkZ) > renderDistance) {
            it = activeChunks.erase(it);
        }
        else {
            ++it;
        }
    }

    for (int x = currentChunkX - renderDistance; x <= currentChunkX + renderDistance; ++x) {
        for (int z = currentChunkZ - renderDistance; z <= currentChunkZ + renderDistance; ++z) {
            std::pair<int, int> chunkPos = { x, z };
            if (activeChunks.find(chunkPos) == activeChunks.end()) {
                auto newChunk = std::make_unique<Chunk>(x, z, this, wallMaterial, floorMaterial, ceilingMaterial);
                newChunk->GeneratePass1_Structure();
                newChunk->GeneratePass4_Props();
                newChunk->BuildMesh();
                activeChunks[chunkPos] = std::move(newChunk);
            }
        }
    }

    for (auto& pair : activeChunks) {
        for (auto& prop : pair.second->props) {

            if (prop.propType == 3 && prop.isBouncing) {

                //gravity
                prop.velocityY -= 9.8f * deltaTime;

                prop.position.y += prop.velocityY * deltaTime;

                if (prop.position.y <= prop.startY) {
                    prop.position.y = prop.startY; 
                    prop.velocityY *= -0.70f;      // turning speed up

                    if (std::abs(prop.velocityY) < 0.6f) {
                        prop.isBouncing = false;
                        prop.velocityY = 0.0f;
                    }
                }
            }
        }
    }
}

void Maze::Draw(const glm::mat4& view, const glm::mat4& proj) {
    for (auto& pair : activeChunks) {
        pair.second->Draw(view, proj);
    }
}

bool Maze::IsSolid(float worldX, float worldZ) const {
    int gridX = (int)glm::floor(worldX / CELL_SIZE);
    int gridZ = (int)glm::floor(worldZ / CELL_SIZE);
    return GetCellTypeGlobal(gridX, gridZ) == CellType::WALL;
}

std::vector<LightData> Maze::GetClosestLamps(const glm::vec3& playerPos, int maxLamps) const {
    std::vector<LightData> allLamps;

    //collecting all lamps
    for (const auto& pair : activeChunks) {
        allLamps.insert(allLamps.end(), pair.second->localLamps.begin(), pair.second->localLamps.end());
    }

    //sorting due to distance
    std::sort(allLamps.begin(), allLamps.end(), [&playerPos](const LightData& a, const LightData& b) {
    
        glm::vec3 diffA = a.position - playerPos;
        glm::vec3 diffB = b.position - playerPos;

        float distSqA = glm::dot(diffA, diffA);
        float distSqB = glm::dot(diffB, diffB);

        return distSqA < distSqB;
        });

    if (allLamps.size() > maxLamps) {
        allLamps.resize(maxLamps);
    }

    return allLamps;
}

bool Maze::CheckCollision(glm::vec3 pos, float playerRadius) const {
    //check corners
    if (IsSolid(pos.x + playerRadius, pos.z + playerRadius)) return true;
    if (IsSolid(pos.x - playerRadius, pos.z + playerRadius)) return true;
    if (IsSolid(pos.x + playerRadius, pos.z - playerRadius)) return true;
    if (IsSolid(pos.x - playerRadius, pos.z - playerRadius)) return true;

    //check furniture
    for (const auto& pair : activeChunks) {
        for (const auto& prop : pair.second->props) {
            
            if (prop.collisionRadius > 0.0f) {
                // calculate dist between object and player
                float dist = glm::distance(glm::vec2(pos.x, pos.z), glm::vec2(prop.position.x, prop.position.z));
                
                if (dist < (playerRadius + prop.collisionRadius)) {
                    return true;
                }
            }
        }
    }
    return false; 
}

bool Maze::CheckLineOfSight(glm::vec3 start, glm::vec3 end) const {
    glm::vec3 dir = end - start;
    float dist = glm::length(dir);
    dir = glm::normalize(dir);

    float stepSize = 0.5f;
    float currentDist = 0.0f;

    while (currentDist < dist) {
        glm::vec3 checkPos = start + dir * currentDist;
        if (IsSolid(checkPos.x, checkPos.z)) {
            return false;
        }
        currentDist += stepSize;
    }
    return true;
}

void Maze::Interact(const glm::vec3& playerPos, const glm::vec3& cameraFront) {
    for (auto& pair : activeChunks) {
        auto& propsList = pair.second->props; 

        for (size_t i = 0; i < propsList.size(); ++i) {
            auto& prop = propsList[i];

            if (prop.isInteractable && !prop.isHidden) {
                glm::vec3 dirToProp = glm::normalize(prop.position - playerPos);
                float dotProduct = glm::dot(cameraFront, dirToProp);
                float distance = glm::distance(playerPos, prop.position);

                if (distance < 3.0f && dotProduct > 0.95f) {

                    //comp
                    if (prop.propType == 1) {
                        prop.isActive = !prop.isActive;
                        std::cout << "Computer toggled!" << std::endl;
                    }

                    //box
                    else if (prop.propType == 2) {
                  
                        propsList.erase(propsList.begin() + i);
                        std::cout << "Box destroyed!" << std::endl;
                    }
                    //ball
                    else if (prop.propType == 3) { 
                        if (!prop.isBouncing) { 
                            prop.isBouncing = true;
                            prop.velocityY = 4.5f;
                            std::cout << "Ball kicked!" << std::endl;
                        }
                    }
                    return;
                }
            }
        }
    }
}
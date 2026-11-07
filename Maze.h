

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include "Structures.h"

constexpr int CHUNK_SIZE = 16;
constexpr float CELL_SIZE = 1.0f;
constexpr float WALL_HEIGHT = 3.2f;

//types of cell in the mase
enum class CellType {
    EMPTY,   
    WALL,   
    DOORWAY 
};

class Chunk; 
float GetHash(int x, int z, unsigned int seed);


/**
 * @brief Manages the infinite procedural maze using Spatial Partitioning.
 * * The Maze class is responsible for generating the world deterministically using a seed.
 * It dynamically loads and unloads 16x16 Chunks based on the player's position to optimize
 * memory and CPU/GPU usage. It also handles global collisions and interactions.
 */
class Maze {
public:
   
    Maze(Material* wallMat, Material* floorMat, Material* ceilingMat, unsigned int seed);
    ~Maze();

    //upload chunks around player and delete chunks that are far away
    /**
     * @brief Updates the active chunks and processes physics.
     * * Calculates the player's current chunk. Unloads chunks that are farther than
     * 'renderDistance' and instantiates new chunks if the player moves to an unexplored area.
     * Also applies gravity to bouncing props.
     * * @param playerPos Current 3D position of the player in the world.
     * @param cameraFront The direction vector the player is currently looking at.
     * @param deltaTime Time elapsed since the last frame.
     */
    void Update(const glm::vec3& playerPos, const glm::vec3& cameraFront, float deltaTime);

    bool CheckLineOfSight(glm::vec3 start, glm::vec3 end) const;

    unsigned int GetSeed() const { return globalSeed; }

    //draw active chunks
    void Draw(const glm::mat4& view, const glm::mat4& proj);

    //calculates type of the cell, not depending on loaded chunks
    /**
     * @brief Deterministically calculates what should be at a specific global coordinate.
     * * Uses hash functions to decide if a cell is a WALL, EMPTY, or DOORWAY without
     * needing the chunk to be loaded in memory.
     * * @param worldX Global X coordinate on the grid.
     * @param worldZ Global Z coordinate on the grid.
     * @return CellType enum representing the structure at this location.
     */
    CellType GetCellTypeGlobal(int worldX, int worldZ) const;

    bool IsSolid(float worldX, float worldZ) const;

    /**
     * @brief Checks if the player collides with walls or props.
     * @param pos The center position of the player.
     * @param playerRadius The collision radius of the player cylinder.
     * @return true if a collision is detected, false otherwise.
     */
    bool CheckCollision(glm::vec3 pos, float playerRadius) const;

    /**
     * @brief Handles player interaction with props (e.g., kicking a ball, toggling a PC).
     * * Uses dot product to ensure the player is looking directly at the object,
     * and distance checking to ensure it is within reach.
     * * @param playerPos Current position of the player.
     * @param cameraFront The direction the player's camera is facing.
     */
    void Interact(const glm::vec3& playerPos, const glm::vec3& cameraFront);

    std::vector<LightData> GetClosestLamps(const glm::vec3& playerPos, int maxLamps) const;

    int renderDistance;
private:
    Material* wallMaterial;
    Material* floorMaterial;
    Material* ceilingMaterial;
    unsigned int globalSeed;

    // key - (chunkX, chunkZ)
    std::map<std::pair<int, int>, std::unique_ptr<Chunk>> activeChunks;

};
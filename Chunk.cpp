#include "Chunk.h"

Chunk::Chunk(int gridX, int gridZ, Maze* parentMaze, Material* wallMat, Material* floorMat, Material* ceilingMat)
    : Object(wallMat), chunkX(gridX), chunkZ(gridZ), maze(parentMaze),
    wallMaterial(wallMat), floorMaterial(floorMat), ceilingMaterial(ceilingMat) {
}

Chunk::~Chunk() {}



// Iterates through the local 16x16 grid and populates it by querying
// the deterministic hash-based global cell type function from the parent maze.
void Chunk::GeneratePass1_Structure() {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;
            grid[x][z] = maze->GetCellTypeGlobal(worldX, worldZ);
        }
    }
}



// Checks if a specific local coordinate is a solid wall.
// If the requested coordinates are outside the boundaries of this chunk,
// it queries the global maze to ensure seamless collision and meshing between chunks.
// localX The local X coordinate within the chunk.
 
bool Chunk::IsSolid(int localX, int localZ) const {
    if (localX >= 0 && localX < CHUNK_SIZE && localZ >= 0 && localZ < CHUNK_SIZE) {
        return grid[localX][localZ] == CellType::WALL;
    }
    int worldX = chunkX * CHUNK_SIZE + localX;
    int worldZ = chunkZ * CHUNK_SIZE + localZ;
    return maze->GetCellTypeGlobal(worldX, worldZ) == CellType::WALL;
}

CellType Chunk::GetLocalCell(int localX, int localZ) const {
    return grid[localX][localZ];
}

// Helper function to add a quad to the mesh geometry.
// Automatically calculates and assigns texture coordinates 
void Chunk::AddQuad(std::vector<Vertex>& verts, std::vector<unsigned int>& idx,
    glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal) {
    unsigned int startIndex = verts.size();
    const float UV_SCALE = 0.25f;

    auto calcUV = [UV_SCALE](glm::vec3 pos, glm::vec3 norm) -> glm::vec2 {
        if (std::abs(norm.y) > 0.5f) return glm::vec2(pos.x, pos.z) * UV_SCALE;
        else if (std::abs(norm.x) > 0.5f) return glm::vec2(pos.z, pos.y) * UV_SCALE;
        else return glm::vec2(pos.x, pos.y) * UV_SCALE;
        };

    verts.push_back({ p0, normal, calcUV(p0, normal) });
    verts.push_back({ p1, normal, calcUV(p1, normal) });
    verts.push_back({ p2, normal, calcUV(p2, normal) });
    verts.push_back({ p3, normal, calcUV(p3, normal) });

    idx.push_back(startIndex + 0); idx.push_back(startIndex + 1); idx.push_back(startIndex + 2);
    idx.push_back(startIndex + 2); idx.push_back(startIndex + 3); idx.push_back(startIndex + 0);
}

void Chunk::AddLintel(std::vector<Vertex>& verts, std::vector<unsigned int>& idx, int localX, int localZ) {
    float startX = chunkX * CHUNK_SIZE * CELL_SIZE;
    float startZ = chunkZ * CHUNK_SIZE * CELL_SIZE;

    float x0 = startX + localX * CELL_SIZE;
    float z0 = startZ + localZ * CELL_SIZE;
    float x1 = x0 + CELL_SIZE;
    float z1 = z0 + CELL_SIZE;

    //where lintel starts
    float doorHeight = 2.85f;

    auto isVoid = [&](int dx, int dz) {
        int wx = chunkX * CHUNK_SIZE + localX + dx;
        int wz = chunkZ * CHUNK_SIZE + localZ + dz;
        return maze->GetCellTypeGlobal(wx, wz) == CellType::EMPTY;
        };

    AddQuad(verts, idx,
        glm::vec3(x0, doorHeight, z0), glm::vec3(x1, doorHeight, z0),
        glm::vec3(x1, doorHeight, z1), glm::vec3(x0, doorHeight, z1),
        glm::vec3(0.0f, -1.0f, 0.0f));

    if (isVoid(-1, 0)) AddQuad(verts, idx, glm::vec3(x0, doorHeight, z1), glm::vec3(x0, doorHeight, z0), glm::vec3(x0, WALL_HEIGHT, z0), glm::vec3(x0, WALL_HEIGHT, z1), glm::vec3(-1.0f, 0.0f, 0.0f));
    if (isVoid(1, 0))  AddQuad(verts, idx, glm::vec3(x1, doorHeight, z0), glm::vec3(x1, doorHeight, z1), glm::vec3(x1, WALL_HEIGHT, z1), glm::vec3(x1, WALL_HEIGHT, z0), glm::vec3(1.0f, 0.0f, 0.0f));
    if (isVoid(0, -1)) AddQuad(verts, idx, glm::vec3(x0, doorHeight, z0), glm::vec3(x1, doorHeight, z0), glm::vec3(x1, WALL_HEIGHT, z0), glm::vec3(x0, WALL_HEIGHT, z0), glm::vec3(0.0f, 0.0f, -1.0f));
    if (isVoid(0, 1))  AddQuad(verts, idx, glm::vec3(x1, doorHeight, z1), glm::vec3(x0, doorHeight, z1), glm::vec3(x0, WALL_HEIGHT, z1), glm::vec3(x1, WALL_HEIGHT, z1), glm::vec3(0.0f, 0.0f, 1.0f));
}


// Constructs 3d mesh for 1 chunk. It only generates
// polygons for faces that are exposed to empty spaces, significantly reducing
// the triangle count
void Chunk::BuildMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float startX = chunkX * CHUNK_SIZE * CELL_SIZE;
    float startZ = chunkZ * CHUNK_SIZE * CELL_SIZE;
    float endX = startX + (CHUNK_SIZE * CELL_SIZE);
    float endZ = startZ + (CHUNK_SIZE * CELL_SIZE);

    AddQuad(vertices, indices,
        glm::vec3(startX, 0.0f, startZ), glm::vec3(startX, 0.0f, endZ),
        glm::vec3(endX, 0.0f, endZ), glm::vec3(endX, 0.0f, startZ),
        glm::vec3(0.0f, 1.0f, 0.0f));

    AddQuad(vertices, indices,
        glm::vec3(startX, WALL_HEIGHT, startZ), glm::vec3(endX, WALL_HEIGHT, startZ),
        glm::vec3(endX, WALL_HEIGHT, endZ), glm::vec3(startX, WALL_HEIGHT, endZ),
        glm::vec3(0.0f, -1.0f, 0.0f));

    this->floorCeilingIndicesCount = (unsigned int)indices.size();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            if (grid[x][z] == CellType::WALL) {
                float x0 = startX + x * CELL_SIZE;
                float z0 = startZ + z * CELL_SIZE;
                float x1 = x0 + CELL_SIZE;
                float z1 = z0 + CELL_SIZE;

                if (!IsSolid(x - 1, z)) AddQuad(vertices, indices, glm::vec3(x0, 0, z1), glm::vec3(x0, 0, z0), glm::vec3(x0, WALL_HEIGHT, z0), glm::vec3(x0, WALL_HEIGHT, z1), glm::vec3(-1.0f, 0.0f, 0.0f));
                if (!IsSolid(x + 1, z)) AddQuad(vertices, indices, glm::vec3(x1, 0, z0), glm::vec3(x1, 0, z1), glm::vec3(x1, WALL_HEIGHT, z1), glm::vec3(x1, WALL_HEIGHT, z0), glm::vec3(1.0f, 0.0f, 0.0f));
                if (!IsSolid(x, z - 1)) AddQuad(vertices, indices, glm::vec3(x0, 0, z0), glm::vec3(x1, 0, z0), glm::vec3(x1, WALL_HEIGHT, z0), glm::vec3(x0, WALL_HEIGHT, z0), glm::vec3(0.0f, 0.0f, -1.0f));
                if (!IsSolid(x, z + 1)) AddQuad(vertices, indices, glm::vec3(x1, 0, z1), glm::vec3(x0, 0, z1), glm::vec3(x0, WALL_HEIGHT, z1), glm::vec3(x1, WALL_HEIGHT, z1), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            else if (grid[x][z] == CellType::DOORWAY) {
                AddLintel(vertices, indices, x, z);
            }

        }
    }

    if (!geometry) geometry = new Geometry();
    if (geometry->vao == 0) glGenVertexArrays(1, &geometry->vao);
    if (geometry->vbo == 0) glGenBuffers(1, &geometry->vbo);
    if (geometry->ebo == 0) glGenBuffers(1, &geometry->ebo);

    glBindVertexArray(geometry->vao);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    if (material && material->shader && material->shader->initialized) {
        ShaderProgram* shdr = material->shader;
        if (shdr->posLocation != -1) {
            glEnableVertexAttribArray(shdr->posLocation);
            glVertexAttribPointer(shdr->posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        }
        if (shdr->normalLocation != -1) {
            glEnableVertexAttribArray(shdr->normalLocation);
            glVertexAttribPointer(shdr->normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        }
        if (shdr->texCoordLocation != -1) {
            glEnableVertexAttribArray(shdr->texCoordLocation);
            glVertexAttribPointer(shdr->texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        }
    }

    glBindVertexArray(0);
    geometry->numTriangles = indices.size() / 3;
    initialized = true;
}

void Chunk::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && material && material->shader && material->shader->initialized) {
        ShaderProgram* shdr = material->shader;
        glUseProgram(shdr->program);

        glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * modelMatrix;
        if (shdr->PVMLocation != -1) glUniformMatrix4fv(shdr->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
        glUniformMatrix4fv(shdr->modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glBindVertexArray(geometry->vao);

        glUniform1i(shdr->hasTextureLocation, 0);
        glUniform3f(shdr->objectColorLocation, 0.3f, 0.25f, 0.15f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (ceilingMaterial && ceilingMaterial->textureID != 0) {
            glUniform1i(shdr->hasTextureLocation, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ceilingMaterial->textureID);
            glUniform1i(shdr->texSamplerLocation, 0);
            glUniform3f(shdr->objectColorLocation, 0.9f, 0.9f, 0.9f);

            void* ceilOffset = (void*)(uintptr_t)(6 * sizeof(unsigned int));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, ceilOffset);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (material->textureID != 0) {
      
            glUniform1i(shdr->hasTextureLocation, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->textureID);
            glUniform1i(shdr->texSamplerLocation, 0);
            glUniform3f(shdr->objectColorLocation, 0.8f, 0.7f, 0.5f);

            if (material->dirtTextureID != 0) {
                glUniform1i(shdr->hasDirtLocation, 1);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material->dirtTextureID);
                glUniform1i(shdr->dirtSamplerLocation, 2);
            }
            else {
                glUniform1i(shdr->hasDirtLocation, 0);
            }

            void* offset = (void*)(uintptr_t)(12 * sizeof(unsigned int));
            unsigned int wallIndicesCount = (geometry->numTriangles * 3) - 12;
            glDrawElements(GL_TRIANGLES, wallIndicesCount, GL_UNSIGNED_INT, offset);

            glUniform1i(shdr->hasDirtLocation, 0);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    for (const auto& prop : props) {
        if (prop.modelTemplate != nullptr) {
            if (prop.propType == 1 && screenShaderProgram.initialized) {
                glUseProgram(screenShaderProgram.program);
                glUniform1i(screenShaderProgram.isScreenOnLocation, prop.isActive ? 1 : 0);
            }

            prop.modelTemplate->SetModelMatrix(prop.GetModelMatrix());
            prop.modelTemplate->Draw(viewMatrix, projectionMatrix);
        }
    }
}


// Procedurally populates the chunk with props and lighting.
// Uses the deterministic hash function from the maze to spawn objects
// (tables, chairs, lamps, boxes, balls) in specific patterns based on
// the topology of the room (corners, near the wall)
void Chunk::GeneratePass4_Props() {
    float startX = chunkX * CHUNK_SIZE * CELL_SIZE;
    float startZ = chunkZ * CHUNK_SIZE * CELL_SIZE;

  
    auto IsSpaceFree = [&](glm::vec3 pos, float radius) {
        for (const auto& p : props) {
   
            if (p.collisionRadius > 0.0f) {
                float dist = glm::distance(glm::vec2(pos.x, pos.z), glm::vec2(p.position.x, p.position.z));
           
                if (dist < (radius + p.collisionRadius) * 0.85f) {
                    return false;
                }
            }
        }
        return true; //free
        };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            if (grid[x][z] == CellType::EMPTY) {
                int worldX = chunkX * CHUNK_SIZE + x;
                int worldZ = chunkZ * CHUNK_SIZE + z;

                float cellCenterX = startX + x * CELL_SIZE + (CELL_SIZE / 2.0f);
                float cellCenterZ = startZ + z * CELL_SIZE + (CELL_SIZE / 2.0f);

                //lamps
                float lampHash = std::abs(GetHash(worldX + 532, worldZ + 901, maze->GetSeed()));
                if (lampHash < 0.03f && lamp) {
                    glm::vec3 lightPos(cellCenterX, WALL_HEIGHT - 0.8f, cellCenterZ);
                    LightData lData;
                    lData.position = lightPos;
                    lData.baseIntensity = 1.8f;
                    lData.isFlickering = (GetHash(worldX, worldZ, maze->GetSeed() + 999) > 0.5f);
                    lData.phaseOffset = GetHash(worldX, worldZ, maze->GetSeed() + 111) * 100.0f;
                    localLamps.push_back(lData);

                    Prop lampProp;
                    lampProp.modelTemplate = lamp.get();
                    lampProp.position = glm::vec3(cellCenterX, WALL_HEIGHT, cellCenterZ);
                    lampProp.rotationY = 0.0f;
                    lampProp.scale = glm::vec3(0.013f);
                    lampProp.collisionRadius = 0.0f;
                    props.push_back(lampProp);
                }

                bool wN = maze->GetCellTypeGlobal(worldX, worldZ - 1) == CellType::WALL;
                bool wS = maze->GetCellTypeGlobal(worldX, worldZ + 1) == CellType::WALL;
                bool wW = maze->GetCellTypeGlobal(worldX - 1, worldZ) == CellType::WALL;
                bool wE = maze->GetCellTypeGlobal(worldX + 1, worldZ) == CellType::WALL;

                int wallCount = wN + wS + wW + wE;
                bool isCorner = (wN && wE) || (wN && wW) || (wS && wE) || (wS && wW);
                float spawnRoll = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 100));

                //objects in the corner
                if (isCorner) {
                    float cornerRoll = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 888));

                    //plant
                    if (cornerRoll < 0.30f && plant) {
                        Prop newPlant;
                        newPlant.modelTemplate = plant.get();
                        newPlant.scale = glm::vec3(0.012f);
                        newPlant.collisionRadius = 0.3f;
                        newPlant.position = glm::vec3(cellCenterX, 0.0f, cellCenterZ);
                        newPlant.rotationY = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 400)) * 360.0f;

                        //check if free
                        if (IsSpaceFree(newPlant.position, newPlant.collisionRadius)) {
                            props.push_back(newPlant);
                        }
                    }
                    // box
                    else if (cornerRoll >= 0.30f && cornerRoll < 0.60f && box) {

                        float pushBoxX = 0.0f;
                        float pushBoxZ = 0.0f;
                        float wallOffset = 0.35f;

                        if (wN) pushBoxZ += wallOffset;
                        if (wS) pushBoxZ -= wallOffset; 
                        if (wE) pushBoxX -= wallOffset; 
                        if (wW) pushBoxX += wallOffset; 

                      
                        glm::vec3 boxBasePos = glm::vec3(cellCenterX + pushBoxX, 0.0f, cellCenterZ + pushBoxZ);

                        if (IsSpaceFree(boxBasePos, 0.4f)) {
                            int boxCount = 1 + (int)(std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 889)) * 3.0f);
                            float currentY = 0.2f;

                            for (int b = 0; b < boxCount; b++) {
                                Prop newBox;
                                newBox.modelTemplate = box.get();
                                newBox.scale = glm::vec3(1.0f);
                                newBox.collisionRadius = 0.4f;
                                newBox.isInteractable = true;
                                newBox.propType = 2; // box id

                                float shiftX = (std::abs(GetHash(worldX, b, maze->GetSeed() + 900)) - 0.5f) * 0.15f;
                                float shiftZ = (std::abs(GetHash(worldX, b, maze->GetSeed() + 901)) - 0.5f) * 0.15f;

                                //add sdvig to the position
                                newBox.position = glm::vec3(boxBasePos.x + shiftX, currentY, boxBasePos.z + shiftZ);
                                newBox.rotationY = std::abs(GetHash(worldX, b, maze->GetSeed() + 902)) * 360.0f;
                                props.push_back(newBox);

                                currentY += 0.385f;
                            }
                        }
                    }
                }
                // near wall
                else if (wallCount == 1) {
                    if (spawnRoll < 0.008f && table) {
                        Prop newTable;
                        newTable.modelTemplate = table.get();
                        newTable.position = glm::vec3(cellCenterX, 0.50f, cellCenterZ);
                        newTable.scale = glm::vec3(0.350f);
                        newTable.collisionRadius = 1.2f; 

                        float faceAngle = 0.0f;
                        float pushX = 0.0f, pushZ = 0.0f;
                        float chairPushX = 0.0f, chairPushZ = 0.0f;
                        float compAngle = 0.0f;

                        float tableOffset = -0.4f;
                        float chairOffset = 1.4f;

                        if (wN) { faceAngle = 90.0f; pushZ = -tableOffset; chairPushZ = chairOffset; compAngle = -90.0f; }
                        else if (wS) { faceAngle = 90.0f; pushZ = tableOffset; chairPushZ = -chairOffset; compAngle = 90.0f; }
                        else if (wE) { faceAngle = 0.0f; pushX = tableOffset; chairPushX = -chairOffset; compAngle = 180.0f; }
                        else if (wW) { faceAngle = 0.0f; pushX = -tableOffset; chairPushX = chairOffset; compAngle = 0.0f; }

                        newTable.position.x += pushX;
                        newTable.position.z += pushZ;
                        newTable.rotationY = faceAngle;

         
                        if (IsSpaceFree(newTable.position, newTable.collisionRadius)) {
                            props.push_back(newTable);

                            //computa 50%
                            if (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 200)) < 0.5f && comp) {
                                Prop newComp;
                                newComp.modelTemplate = comp.get();
                                newComp.position = glm::vec3(newTable.position.x, 1.01f, newTable.position.z);
                                newComp.rotationY = compAngle;
                                newComp.scale = glm::vec3(0.28f);
                                newComp.collisionRadius = 0.0f;

                                newComp.isInteractable = true;
                                newComp.isActive = true; 
                                newComp.propType = 1;    //comp id
                                props.push_back(newComp);
                            }

                            //chair 80%
                            if (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 300)) < 0.8f && chair) {
                                Prop tChair;
                                tChair.modelTemplate = chair.get();
                                tChair.scale = glm::vec3(0.015f);
                                tChair.collisionRadius = 0.25f;
                                tChair.position = glm::vec3(newTable.position.x + chairPushX, 0.0f, newTable.position.z + chairPushZ);
                                tChair.rotationY = compAngle - 90.0f;

                               
                                if (IsSpaceFree(tChair.position, tChair.collisionRadius)) {
                                    props.push_back(tChair);
                                }
                            }

                            //dustbin 50%
                            if (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 800)) < 0.5f && dustbin) {
                                Prop newDustbin;
                                newDustbin.modelTemplate = dustbin.get();
                                newDustbin.scale = glm::vec3(0.35f);
                                newDustbin.collisionRadius = 0.2f;
                                newDustbin.position = glm::vec3(newTable.position.x + chairPushZ * 0.8f, 0.0f, newTable.position.z + chairPushX * 0.8f);
                                newDustbin.rotationY = 0.0f;

                                if (IsSpaceFree(newDustbin.position, newDustbin.collisionRadius)) {
                                    props.push_back(newDustbin);
                                }
                            }
                        }
                    }
                }
                //everywhere
                else if (wallCount == 0 || wallCount == 2) {

                    float ballRoll = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 777));
                    if (ballRoll < 0.005f && ball) {
                        Prop newBall;
                        newBall.modelTemplate = ball.get();
                        newBall.scale = glm::vec3(0.35f);
                        newBall.collisionRadius = 0.2f;

                      
                        float offsetX = (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 111)) - 0.5f) * 1.5f;
                        float offsetZ = (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 222)) - 0.5f) * 1.5f;

                    
                        newBall.position = glm::vec3(cellCenterX + offsetX, 0.15f, cellCenterZ + offsetZ);
                        newBall.rotationY = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 333)) * 360.0f;

                        newBall.isInteractable = true;
                        newBall.propType = 3;   // id 3
                        newBall.startY = 0.15f; // floor

                        if (IsSpaceFree(newBall.position, newBall.collisionRadius)) {
                            props.push_back(newBall);
                        }
                    }

                    if (spawnRoll < 0.008f && chair && wallCount == 0) {
                        Prop loneChair;
                        loneChair.modelTemplate = chair.get();
                        loneChair.scale = glm::vec3(0.015f);
                        loneChair.collisionRadius = 0.25f;
                        float offsetX = (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 501)) - 0.5f) * 1.0f;
                        float offsetZ = (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 502)) - 0.5f) * 1.0f;
                        loneChair.position = glm::vec3(cellCenterX + offsetX, 0.0f, cellCenterZ + offsetZ);
                        loneChair.rotationY = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 503)) * 360.0f;

                        if (IsSpaceFree(loneChair.position, loneChair.collisionRadius)) {
                            props.push_back(loneChair);

                            //second chair nearby
                            float secondChairRoll = std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 600));
                            if (secondChairRoll < 0.15f) {
                                Prop secondChair = loneChair;
                                float angleRad = glm::radians(loneChair.rotationY);
                                float shiftX = glm::cos(angleRad) * 0.8f;
                                float shiftZ = -glm::sin(angleRad) * 0.8f;
                                secondChair.position.x += shiftX;
                                secondChair.position.z += shiftZ;
                                secondChair.rotationY += (std::abs(GetHash(worldX, worldZ, maze->GetSeed() + 601)) - 0.5f) * 30.0f;

                                if (IsSpaceFree(secondChair.position, secondChair.collisionRadius)) {
                                    props.push_back(secondChair);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
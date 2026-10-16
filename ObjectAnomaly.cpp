#include "Objects.h"

ObjectAnomaly::ObjectAnomaly(Material* mat, glm::vec3 startPosition) : Object(mat) {
    startPos = startPosition;
    pos = startPos;
    scaleFactors = glm::vec3(0.3f);
    modelMatrix = glm::mat4(1.0f);
    geometry = new Geometry;

    const float h = 1.5f; 
    const float hb = -1.5f; 

    const float c1 = 0.924f, s1 = 0.383f; // 22.5 
    const float c2 = 0.707f, s2 = 0.707f; // 45.0 
    const float c3 = 0.383f, s3 = 0.924f; // 67.5 

    const float ny = 0.866f; // up
    const float nyb = -0.866f; // down
    const float n1c = 0.449f, n1s = 0.207f;
    const float n2c = 0.351f, n2s = 0.351f;
    const float n3 = 0.500f;

    static const Vertex anomalyVertices[] = {
        { {0.0f, h, 0.0f}, {n2c, ny, n2c}, {0.5f, 1.0f} }, { {1.0f, 0.0f, 0.0f}, {n2c, ny, n2c}, {1.0f, 0.0f} }, { {c1, 0.0f, s1}, {n2c, ny, n2c}, {c1, s1} },
        { {0.0f, h, 0.0f}, {n1s, ny, n1c}, {0.5f, 1.0f} }, { {c1, 0.0f, s1}, {n1s, ny, n1c}, {c1, s1} }, { {c2, 0.0f, s2}, {n1s, ny, n1c}, {c2, s2} },
        { {0.0f, h, 0.0f}, {0.0f, ny, n3 }, {0.5f, 1.0f} }, { {c2, 0.0f, s2}, {0.0f, ny, n3 }, {c2, s2} }, { {c3, 0.0f, s3}, {0.0f, ny, n3 }, {c3, s3} },
        { {0.0f, h, 0.0f}, {-n1s, ny, n1c}, {0.5f, 1.0f} }, { {c3, 0.0f, s3}, {-n1s, ny, n1c}, {c3, s3} }, { {0.0f, 0.0f, 1.0f}, {-n1s, ny, n1c}, {0.0f, 1.0f} },

        { {0.0f, h, 0.0f}, {-n2c, ny, n2c}, {0.5f, 1.0f} }, { {0.0f, 0.0f, 1.0f}, {-n2c, ny, n2c}, {0.0f, 1.0f} }, { {-c3, 0.0f, s3}, {-n2c, ny, n2c}, {-c3, s3} },
        { {0.0f, h, 0.0f}, {-n1c, ny, n1s}, {0.5f, 1.0f} }, { {-c3, 0.0f, s3}, {-n1c, ny, n1s}, {-c3, s3} }, { {-c2, 0.0f, s2}, {-n1c, ny, n1s}, {-c2, s2} },
        { {0.0f, h, 0.0f}, {-n3, ny, 0.0f}, {0.5f, 1.0f} }, { {-c2, 0.0f, s2}, {-n3, ny, 0.0f}, {-c2, s2} }, { {-c1, 0.0f, s1}, {-n3, ny, 0.0f}, {-c1, s1} },
        { {0.0f, h, 0.0f}, {-n1c, ny, -n1s}, {0.5f, 1.0f} }, { {-c1, 0.0f, s1}, {-n1c, ny, -n1s}, {-c1, s1} }, { {-1.0f, 0.0f, 0.0f}, {-n1c, ny, -n1s}, {-1.0f, 0.0f} },

        { {0.0f, h, 0.0f}, {-n2c, ny, -n2c}, {0.5f, 1.0f} }, { {-1.0f, 0.0f, 0.0f}, {-n2c, ny, -n2c}, {-1.0f, 0.0f} }, { {-c1, 0.0f, -s1}, {-n2c, ny, -n2c}, {-c1, -s1} },
        { {0.0f, h, 0.0f}, {-n1s, ny, -n1c}, {0.5f, 1.0f} }, { {-c1, 0.0f, -s1}, {-n1s, ny, -n1c}, {-c1, -s1} }, { {-c2, 0.0f, -s2}, {-n1s, ny, -n1c}, {-c2, -s2} },
        { {0.0f, h, 0.0f}, {0.0f, ny, -n3}, {0.5f, 1.0f} }, { {-c2, 0.0f, -s2}, {0.0f, ny, -n3}, {-c2, -s2} }, { {-c3, 0.0f, -s3}, {0.0f, ny, -n3}, {-c3, -s3} },
        { {0.0f, h, 0.0f}, {n1s, ny, -n1c}, {0.5f, 1.0f} }, { {-c3, 0.0f, -s3}, {n1s, ny, -n1c}, {-c3, -s3} }, { {0.0f, 0.0f, -1.0f}, {n1s, ny, -n1c}, {0.0f, -1.0f} },

        { {0.0f, h, 0.0f}, {n2c, ny, -n2c}, {0.5f, 1.0f} }, { {0.0f, 0.0f, -1.0f}, {n2c, ny, -n2c}, {0.0f, -1.0f} }, { {c3, 0.0f, -s3}, {n2c, ny, -n2c}, {c3, -s3} },
        { {0.0f, h, 0.0f}, {n1c, ny, -n1s}, {0.5f, 1.0f} }, { {c3, 0.0f, -s3}, {n1c, ny, -n1s}, {c3, -s3} }, { {c2, 0.0f, -s2}, {n1c, ny, -n1s}, {c2, -s2} },
        { {0.0f, h, 0.0f}, {n3, ny, 0.0f}, {0.5f, 1.0f} }, { {c2, 0.0f, -s2}, {n3, ny, 0.0f}, {c2, -s2} }, { {c1, 0.0f, -s1}, {n3, ny, 0.0f}, {c1, -s1} },
        { {0.0f, h, 0.0f}, {n1c, ny, n1s}, {0.5f, 1.0f} }, { {c1, 0.0f, -s1}, {n1c, ny, n1s}, {c1, -s1} }, { {1.0f, 0.0f, 0.0f}, {n1c, ny, n1s}, {1.0f, 0.0f} },

   
        { {0.0f, hb, 0.0f}, {n2c, nyb, n2c}, {0.5f, 0.0f} }, { {c1, 0.0f, s1}, {n2c, nyb, n2c}, {c1, s1} }, { {1.0f, 0.0f, 0.0f}, {n2c, nyb, n2c}, {1.0f, 0.0f} },
        { {0.0f, hb, 0.0f}, {n1s, nyb, n1c}, {0.5f, 0.0f} }, { {c2, 0.0f, s2}, {n1s, nyb, n1c}, {c2, s2} }, { {c1, 0.0f, s1}, {n1s, nyb, n1c}, {c1, s1} },
        { {0.0f, hb, 0.0f}, {0.0f, nyb, n3 }, {0.5f, 0.0f} }, { {c3, 0.0f, s3}, {0.0f, nyb, n3 }, {c3, s3} }, { {c2, 0.0f, s2}, {0.0f, nyb, n3 }, {c2, s2} },
        { {0.0f, hb, 0.0f}, {-n1s, nyb, n1c}, {0.5f, 0.0f} }, { {0.0f, 0.0f, 1.0f}, {-n1s, nyb, n1c}, {0.0f, 1.0f} }, { {c3, 0.0f, s3}, {-n1s, nyb, n1c}, {c3, s3} },

        { {0.0f, hb, 0.0f}, {-n2c, nyb, n2c}, {0.5f, 0.0f} }, { {-c3, 0.0f, s3}, {-n2c, nyb, n2c}, {-c3, s3} }, { {0.0f, 0.0f, 1.0f}, {-n2c, nyb, n2c}, {0.0f, 1.0f} },
        { {0.0f, hb, 0.0f}, {-n1c, nyb, n1s}, {0.5f, 0.0f} }, { {-c2, 0.0f, s2}, {-n1c, nyb, n1s}, {-c2, s2} }, { {-c3, 0.0f, s3}, {-n1c, nyb, n1s}, {-c3, s3} },
        { {0.0f, hb, 0.0f}, {-n3, nyb, 0.0f}, {0.5f, 0.0f} }, { {-c1, 0.0f, s1}, {-n3, nyb, 0.0f}, {-c1, s1} }, { {-c2, 0.0f, s2}, {-n3, nyb, 0.0f}, {-c2, s2} },
        { {0.0f, hb, 0.0f}, {-n1c, nyb, -n1s}, {0.5f, 0.0f} }, { {-1.0f, 0.0f, 0.0f}, {-n1c, nyb, -n1s}, {-1.0f, 0.0f} }, { {-c1, 0.0f, s1}, {-n1c, nyb, -n1s}, {-c1, s1} },

        { {0.0f, hb, 0.0f}, {-n2c, nyb, -n2c}, {0.5f, 0.0f} }, { {-c1, 0.0f, -s1}, {-n2c, nyb, -n2c}, {-c1, -s1} }, { {-1.0f, 0.0f, 0.0f}, {-n2c, nyb, -n2c}, {-1.0f, 0.0f} },
        { {0.0f, hb, 0.0f}, {-n1s, nyb, -n1c}, {0.5f, 0.0f} }, { {-c2, 0.0f, -s2}, {-n1s, nyb, -n1c}, {-c2, -s2} }, { {-c1, 0.0f, -s1}, {-n1s, nyb, -n1c}, {-c1, -s1} },
        { {0.0f, hb, 0.0f}, {0.0f, nyb, -n3}, {0.5f, 0.0f} }, { {-c3, 0.0f, -s3}, {0.0f, nyb, -n3}, {-c3, -s3} }, { {-c2, 0.0f, -s2}, {0.0f, nyb, -n3}, {-c2, -s2} },
        { {0.0f, hb, 0.0f}, {n1s, nyb, -n1c}, {0.5f, 0.0f} }, { {0.0f, 0.0f, -1.0f}, {n1s, nyb, -n1c}, {0.0f, -1.0f} }, { {-c3, 0.0f, -s3}, {n1s, nyb, -n1c}, {-c3, -s3} },

        { {0.0f, hb, 0.0f}, {n2c, nyb, -n2c}, {0.5f, 0.0f} }, { {c3, 0.0f, -s3}, {n2c, nyb, -n2c}, {c3, -s3} }, { {0.0f, 0.0f, -1.0f}, {n2c, nyb, -n2c}, {0.0f, -1.0f} },
        { {0.0f, hb, 0.0f}, {n1c, nyb, -n1s}, {0.5f, 0.0f} }, { {c2, 0.0f, -s2}, {n1c, nyb, -n1s}, {c2, -s2} }, { {c3, 0.0f, -s3}, {n1c, nyb, -n1s}, {c3, -s3} },
        { {0.0f, hb, 0.0f}, {n3, nyb, 0.0f}, {0.5f, 0.0f} }, { {c1, 0.0f, -s1}, {n3, nyb, 0.0f}, {c1, -s1} }, { {c2, 0.0f, -s2}, {n3, nyb, 0.0f}, {c2, -s2} },
        { {0.0f, hb, 0.0f}, {n1c, nyb, n1s}, {0.5f, 0.0f} }, { {1.0f, 0.0f, 0.0f}, {n1c, nyb, n1s}, {1.0f, 0.0f} }, { {c1, 0.0f, -s1}, {n1c, nyb, n1s}, {c1, -s1} }
    };

    geometry->numTriangles = 32;

    glGenVertexArrays(1, &geometry->vao);
    glBindVertexArray(geometry->vao);

    glGenBuffers(1, &geometry->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(anomalyVertices), anomalyVertices, GL_STATIC_DRAW);

    if (material && material->shader && material->shader->initialized) {
        ShaderProgram* shdr = material->shader;
        glEnableVertexAttribArray(shdr->posLocation);
        glVertexAttribPointer(shdr->posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

        glEnableVertexAttribArray(shdr->normalLocation);
        glVertexAttribPointer(shdr->normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(shdr->texCoordLocation);
        glVertexAttribPointer(shdr->texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        initialized = true;
    }
    glBindVertexArray(0);
}

void ObjectAnomaly::Update(float time) {
    float moveSpeed = 0.5f; 
    float scale = 1.5f;    

    float heightMultiplier = 0.4f;

    float t = time * moveSpeed;
    pos.x = startPos.x + scale * sin(t);
    pos.y = startPos.y + (scale * heightMultiplier) * cos(t * 3.0f);
    pos.z = startPos.z + scale * sin(t * 2.0f);

    glm::vec3 tangent;
    tangent.x = scale * cos(t);
    tangent.y = -(scale * heightMultiplier) * 3.0f * sin(t * 3.0f);
    tangent.z = scale * 2.0f * cos(t * 2.0f);
    tangent = glm::normalize(tangent);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::vec3 right = glm::normalize(glm::cross(worldUp, tangent));
    glm::vec3 up = glm::normalize(glm::cross(tangent, right));

    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix[0] = glm::vec4(right, 0.0f);
    rotationMatrix[1] = glm::vec4(up, 0.0f);
    rotationMatrix[2] = glm::vec4(tangent, 0.0f);

    rotationMatrix = glm::rotate(rotationMatrix, time * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::translate(glm::mat4(1.0f), pos);
    modelMatrix = modelMatrix * rotationMatrix;
    modelMatrix = glm::scale(modelMatrix, scaleFactors);
}

void ObjectAnomaly::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && material && material->shader && material->shader->initialized) {
        glUseProgram(material->shader->program);

        glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * modelMatrix;
        glUniformMatrix4fv(material->shader->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
        glUniformMatrix4fv(material->shader->modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    
        glUniform3f(material->shader->objectColorLocation, 0.9f, 0.1f, 0.3f);
        glUniform1i(material->shader->hasTextureLocation, 0);

        glBindVertexArray(geometry->vao);
   
        glDrawArrays(GL_TRIANGLES, 0, geometry->numTriangles * 3);
        glBindVertexArray(0);
    }
}
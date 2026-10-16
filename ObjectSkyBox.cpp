#include "Objects.h"

ObjectSkybox::ObjectSkybox(Material* mat) : Object(mat) {
    modelMatrix = glm::mat4(1.0f);
    geometry = new Geometry;

    static const Vertex vertices[] = {
        {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(-1, -1, -1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(1, 0)},
        {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(1, 1, -1), glm::vec3(0), glm::vec2(1, 1)}, {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(0, 1)},
     
        {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(-1, -1, -1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(1, 1)},
        {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(1, 1)}, {glm::vec3(-1, 1, 1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 0)},
   
        {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(1, -1, 1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 1)},
        {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 1)}, {glm::vec3(1, 1, -1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(0, 0)},
   
        {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(-1, 1, 1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 1)},
        {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 1)}, {glm::vec3(1, -1, 1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 0)},
  
        {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(1, 1, -1), glm::vec3(0), glm::vec2(1, 1)}, {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 0)},
        {glm::vec3(1, 1, 1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(-1, 1, 1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec2(0, 1)},
    
        {glm::vec3(-1, -1, -1), glm::vec3(0), glm::vec2(0, 0)}, {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(1, 0)},
        {glm::vec3(1, -1, -1), glm::vec3(0), glm::vec2(1, 0)}, {glm::vec3(-1, -1, 1), glm::vec3(0), glm::vec2(0, 1)}, {glm::vec3(1, -1, 1), glm::vec3(0), glm::vec2(1, 1)}
    };

    geometry->numTriangles = 12;
    glGenVertexArrays(1, &geometry->vao);
    glBindVertexArray(geometry->vao);
    glGenBuffers(1, &geometry->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    if (skyboxShaderProgram.initialized) {
        if (skyboxShaderProgram.posLocation != -1) {
            glEnableVertexAttribArray(skyboxShaderProgram.posLocation);
            glVertexAttribPointer(skyboxShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        }
        if (skyboxShaderProgram.texCoordLocation != -1) {
            glEnableVertexAttribArray(skyboxShaderProgram.texCoordLocation);
            glVertexAttribPointer(skyboxShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        }
        initialized = true;
    }
    glBindVertexArray(0);
}

void ObjectSkybox::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && skyboxShaderProgram.initialized) {
        glUseProgram(skyboxShaderProgram.program);

        // deleteing moving coords
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));

        glm::mat4 PVMmatrix = projectionMatrix * viewNoTranslation * glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));

        glUniformMatrix4fv(skyboxShaderProgram.PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));

        glUniform1i(skyboxShaderProgram.texSamplerLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material->textureID);

        glDepthMask(GL_FALSE);

        glBindVertexArray(geometry->vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
    }
}
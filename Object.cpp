#include "Objects.h";

Object::~Object() {
    if (geometry != nullptr) {
        glDeleteVertexArrays(1, &(geometry->vao));
        glDeleteBuffers(1, &(geometry->ebo));
        glDeleteBuffers(1, &(geometry->vbo));
        delete geometry;
        geometry = nullptr;
    }
    initialized = false;
}

void Object::Update() {}

void Object::Hide(bool hide) {
    render = !hide;
}

void Object::SetPosition(glm::vec3 position) {
    this->pos = position;
    modelMatrix[3] = glm::vec4(position, 1.f);
}

glm::vec3 Object::GetPosition() {
    return pos;
}

void Object::SetScale(glm::vec3 scaleFactors) {
    modelMatrix = glm::scale(modelMatrix, scaleFactors);
}

void Object::SetRotation(float angleDegrees, glm::vec3 axis) {
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angleDegrees), axis);
}

ObjectTriangle::ObjectTriangle(Material* mat) : Object(mat) {
    modelMatrix = glm::mat4(1.0f);
    geometry = new Geometry;

    
    static const Vertex vertices[] = {
        //    (x, y, z)           (nx, ny, nz)        UV (u, v)
        { glm::vec3(0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f) }, 
        { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) } 
    };

    geometry->numTriangles = 1;

    glGenVertexArrays(1, &geometry->vao);
    glBindVertexArray(geometry->vao);

    glGenBuffers(1, &geometry->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

        initialized = true;
    }
    else {
        std::cerr << "Triangle Error: Material or Shader is missing/not initialized!" << std::endl;
    }

    glBindVertexArray(0);
}



void ObjectTriangle::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && material && material->shader && material->shader->initialized) {
        ShaderProgram* shdr = material->shader;
        glUseProgram(shdr->program);

        glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * modelMatrix;

        if (shdr->PVMLocation != -1) {
            glUniformMatrix4fv(shdr->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
        }

        if (shdr->modelLocation != -1) {
            glUniformMatrix4fv(shdr->modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        }

        glBindVertexArray(geometry->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& mat) {
    this->material = mat;
    this->geometry.numTriangles = indices.size() / 3;

    glGenVertexArrays(1, &geometry.vao);
    glGenBuffers(1, &geometry.vbo);
    glGenBuffers(1, &geometry.ebo);

    glBindVertexArray(geometry.vao);

    glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    ShaderProgram* shdr = material.shader;
    if (shdr && shdr->posLocation != -1) {
        glEnableVertexAttribArray(shdr->posLocation);
        glVertexAttribPointer(shdr->posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    }
    if (shdr && shdr->normalLocation != -1) {
        glEnableVertexAttribArray(shdr->normalLocation);
        glVertexAttribPointer(shdr->normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
    if (shdr && shdr->texCoordLocation != -1) {
        glEnableVertexAttribArray(shdr->texCoordLocation);
        glVertexAttribPointer(shdr->texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    }

    glBindVertexArray(0);
}

void Mesh::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) {
    if (!material.shader || !material.shader->initialized) return;

    ShaderProgram* shdr = material.shader;
    glUseProgram(shdr->program);

    glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * modelMatrix;

    if (material.shader->program == screenShaderProgram.program) {
        glUniformMatrix4fv(material.shader->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));

        GLuint activeTexture = material.textureID;
        if (activeTexture == 0) {
            activeTexture = material.emissiveTextureID;
        }

        //send right texture to the screen 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, activeTexture);
        glUniform1i(material.shader->texSamplerLocation, 0);
    }
    else {
        glUniformMatrix4fv(shdr->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
        glUniformMatrix4fv(shdr->modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glUniform3fv(shdr->objectColorLocation, 1, glm::value_ptr(material.diffuse));

        if (material.textureID != 0) {
            glUniform1i(shdr->hasTextureLocation, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.textureID);
            glUniform1i(shdr->texSamplerLocation, 0);
        }
        else {
            glUniform1i(shdr->hasTextureLocation, 0);
        }

        if (material.emissiveTextureID != 0) {
            glUniform1i(material.shader->hasEmissiveLocation, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.emissiveTextureID);
            glUniform1i(material.shader->emissiveSamplerLocation, 1);
        }
        else {
            glUniform1i(material.shader->hasEmissiveLocation, 0);
        }

    }
    glBindVertexArray(geometry.vao);
    glDrawElements(GL_TRIANGLES, geometry.numTriangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void Mesh::Cleanup() {
    glDeleteVertexArrays(1, &geometry.vao);
    glDeleteBuffers(1, &geometry.vbo);
    glDeleteBuffers(1, &geometry.ebo);
}


ObjectModel::ObjectModel(const std::string& path) : Object(nullptr) {
    modelMatrix = glm::mat4(1.0f);
    loadModel(path);
}

ObjectModel::~ObjectModel() {
    for (auto& mesh : meshes) {
        mesh.Cleanup();
    }
}

void ObjectModel::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (!render) return;
    for (auto& mesh : meshes) {
        mesh.Draw(viewMatrix, projectionMatrix, modelMatrix);
    }
}

void ObjectModel::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void ObjectModel::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh ObjectModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    material.shader = &shaderProgram; //bind global shader
    material.textureID = 0;

    //verticies
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        //positions
        vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        //normals
        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        //coords
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    //materials
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        aiColor4D color;

        // diffuse colour
        if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
            material.diffuse = glm::vec3(color.r, color.g, color.b);
        }
        else {
            material.diffuse = glm::vec3(0.8f); //default - grey-white
        }

        //loading texture, only the 1st - diffuse
        if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString str;
            aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            std::string texPath = directory + "/" + str.C_Str();

            material.textureID = pgr::createTexture(texPath);
            if (material.textureID == 0) {
                std::cerr << "Failed to load texture: " << texPath << std::endl;
            }
        }
        
        //added emissive for computer mesh
        material.emissiveTextureID = 0; 
        if (aiMat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
            aiString str;
            aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &str);
            std::string filename = str.C_Str();

            size_t lastSlash = filename.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                filename = filename.substr(lastSlash + 1);
            }

            std::string texPath = directory + "/" + filename;
            material.emissiveTextureID = pgr::createTexture(texPath);
        }

        aiString matName;
        aiMat->Get(AI_MATKEY_NAME, matName);

        if (std::string(matName.C_Str()) == "Material.003") { //material name of computer screen
            material.shader = &screenShaderProgram; //use specific shader
        }
        else {
            material.shader = &shaderProgram;
        }

    }

    return Mesh(vertices, indices, material);
}


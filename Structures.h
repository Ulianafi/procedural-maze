#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "pgr.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Config.h"

struct Geometry {
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    unsigned int numTriangles;
};

struct ShaderProgram {
    GLuint program;
    bool initialized;

    GLint posLocation;
    GLint normalLocation; 
    GLint texCoordLocation; 
    GLint PVMLocation;
    GLint modelLocation;
    GLint viewPosLocation;
    GLint timeLocation;

    GLint objectColorLocation;
    GLint hasTextureLocation;
    GLint texSamplerLocation;

    GLint hasEmissiveLocation;    
    GLint emissiveSamplerLocation; 

    GLint dirtSamplerLocation;
    GLint hasDirtLocation;

    GLint isFlashlightOnLocation;
    GLint spotDirLocation;
    GLint spotCutOffLocation;
    GLint spotOuterCutOffLocation;

 
    GLint numPointLightsLocation;
    GLint pointLightPosLocations[20];
    GLint pointLightIntensityLocations[20];


    GLint fogDensityLocation;

    GLint isScreenOnLocation;
    GLint isSkyboxLocation;
};

extern ShaderProgram shaderProgram;
extern ShaderProgram screenShaderProgram;
extern ShaderProgram skyboxShaderProgram;

void initializeShaderPrograms();

struct Material {
    ShaderProgram* shader;
    GLuint textureID; 
    glm::vec3 ambient;    
    glm::vec3 diffuse;    
    glm::vec3 specular;  
    float shininess;      
    GLuint emissiveTextureID = 0; 
    GLuint dirtTextureID = 0;
};

class ObjectModel;

//lightweight structure representing a single item in the world.
//multiple props can share the same heavy 3D ObjectModel to save memory.
/**
 * @brief Lightweight representation of an interactive or decorative object.
 * * Implements the Flyweight (Instancing) pattern. Instead of keeping a full 3D mesh
 * in memory for every object, this struct holds a pointer to a shared ObjectModel template.
 * It only maintains its unique spatial data (position, rotation) and gameplay state.
 */
struct Prop {
    ObjectModel* modelTemplate;
    glm::vec3 position;
    float rotationY;
    glm::vec3 scale;
    float collisionRadius = 0.0f;
    bool isInteractable = false; 
    bool isActive = true;       
    bool isHidden = false;
    int propType = 0;
    float velocityY = 0.0f; 
    bool isBouncing = false;
    float startY = 0.0f; 

    glm::mat4 GetModelMatrix() const {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, scale);
        return m;
    }
};


extern std::unique_ptr<ObjectModel> table;
extern std::unique_ptr<ObjectModel> chair;
extern std::unique_ptr<ObjectModel> plant;
extern std::unique_ptr<ObjectModel> lamp; 
extern std::unique_ptr<ObjectModel> comp;
extern std::unique_ptr<ObjectModel> box;   
extern std::unique_ptr<ObjectModel> dustbin; 
extern std::unique_ptr<ObjectModel> woman;
extern std::unique_ptr<ObjectModel> ball;


//represents a single point in 3D space with attributes for the shader
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

//properties for procedural lamps placed in the maze
struct LightData {
    glm::vec3 position;
    bool isFlickering;
    float baseIntensity;
    float phaseOffset; 
};

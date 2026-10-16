#include "Structures.h"


const std::string colorVertexShaderSrc(
    "#version 140\n"
    "uniform mat4 PVMmatrix;\n"
    "in vec3 position;\n"
    "in vec3 color;\n"
    "smooth out vec4 theColor;\n"
    "void main() {\n"
    "  gl_Position = PVMmatrix * vec4(position, 1.0);\n"
    "  theColor = vec4(color, 1.0);\n"
    "}\n"
);

const std::string colorFragmentShaderSrc(
    "#version 140\n"
    "smooth in vec4 theColor;\n"
    "out vec4 outputColor;\n"
    "void main() {\n"
    "  outputColor = theColor;\n"
    "}\n"
);



ShaderProgram shaderProgram;
ShaderProgram screenShaderProgram;
ShaderProgram skyboxShaderProgram;

void initializeShaderPrograms() {
    std::vector<GLuint> shaderList;

    shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "obec.vert"));
    shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "obec.frag"));

    shaderProgram.program = pgr::createProgram(shaderList);

    shaderProgram.posLocation = glGetAttribLocation(shaderProgram.program, "position");
    shaderProgram.normalLocation = glGetAttribLocation(shaderProgram.program, "normal");
    shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");

    shaderProgram.PVMLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
    shaderProgram.modelLocation = glGetUniformLocation(shaderProgram.program, "modelMatrix");

    shaderProgram.viewPosLocation = glGetUniformLocation(shaderProgram.program, "viewPos");
    shaderProgram.texSamplerLocation = glGetUniformLocation(shaderProgram.program, "texSampler");
    shaderProgram.objectColorLocation = glGetUniformLocation(shaderProgram.program, "objectColor");
    shaderProgram.hasTextureLocation = glGetUniformLocation(shaderProgram.program, "hasTexture");
    shaderProgram.hasEmissiveLocation = glGetUniformLocation(shaderProgram.program, "hasEmissive");
    shaderProgram.emissiveSamplerLocation = glGetUniformLocation(shaderProgram.program, "emissiveSampler");
    shaderProgram.hasDirtLocation = glGetUniformLocation(shaderProgram.program, "hasDirt");
    shaderProgram.dirtSamplerLocation = glGetUniformLocation(shaderProgram.program, "dirtSampler");
    shaderProgram.timeLocation = glGetUniformLocation(shaderProgram.program, "time");

    shaderProgram.isFlashlightOnLocation = glGetUniformLocation(shaderProgram.program, "isFlashlightOn");
    shaderProgram.spotDirLocation = glGetUniformLocation(shaderProgram.program, "spotDir");
    shaderProgram.spotCutOffLocation = glGetUniformLocation(shaderProgram.program, "spotCutOff");
    shaderProgram.spotOuterCutOffLocation = glGetUniformLocation(shaderProgram.program, "spotOuterCutOff");

    shaderProgram.numPointLightsLocation = glGetUniformLocation(shaderProgram.program, "numPointLights");

    for (int i = 0; i < 20; ++i) {
        std::string name = "pointLightPositions[" + std::to_string(i) + "]";
        shaderProgram.pointLightPosLocations[i] = glGetUniformLocation(shaderProgram.program, name.c_str());

        std::string intName = "pointLightIntensities[" + std::to_string(i) + "]";
        shaderProgram.pointLightIntensityLocations[i] = glGetUniformLocation(shaderProgram.program, intName.c_str());

    }
    shaderProgram.fogDensityLocation = glGetUniformLocation(shaderProgram.program, "fogDensity");

    std::vector<GLuint> screenShaders;
    screenShaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "screen.vert"));
    screenShaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "screen.frag"));
    screenShaderProgram.program = pgr::createProgram(screenShaders);

    screenShaderProgram.posLocation = glGetAttribLocation(screenShaderProgram.program, "position");
    screenShaderProgram.texCoordLocation = glGetAttribLocation(screenShaderProgram.program, "texCoord");
    screenShaderProgram.PVMLocation = glGetUniformLocation(screenShaderProgram.program, "PVMmatrix");
    screenShaderProgram.normalLocation = -1;
    screenShaderProgram.texSamplerLocation = glGetUniformLocation(screenShaderProgram.program, "texSampler");
    screenShaderProgram.timeLocation = glGetUniformLocation(screenShaderProgram.program, "time");
    screenShaderProgram.isScreenOnLocation = glGetUniformLocation(screenShaderProgram.program, "isScreenOn");
 
    screenShaderProgram.initialized = true;


    if (shaderProgram.program != 0 && shaderProgram.posLocation != -1 && shaderProgram.PVMLocation != -1) {
        shaderProgram.initialized = true;
    }
    else {
        std::cerr << "Error: Shader locations not found!" << std::endl;
        shaderProgram.initialized = false;
    }

    std::vector<GLuint> skyboxShaders;
    skyboxShaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
    skyboxShaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));
    skyboxShaderProgram.program = pgr::createProgram(skyboxShaders);

    skyboxShaderProgram.posLocation = glGetAttribLocation(skyboxShaderProgram.program, "position");
    skyboxShaderProgram.texCoordLocation = glGetAttribLocation(skyboxShaderProgram.program, "texCoord");
    skyboxShaderProgram.PVMLocation = glGetUniformLocation(skyboxShaderProgram.program, "PVMmatrix");
    skyboxShaderProgram.texSamplerLocation = glGetUniformLocation(skyboxShaderProgram.program, "texSampler");
    skyboxShaderProgram.isSkyboxLocation = glGetUniformLocation(skyboxShaderProgram.program, "isSkybox");

    skyboxShaderProgram.initialized = true;
}

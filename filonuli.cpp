//----------------------------------------------------------------------------------------
/**
 * \file       filonuli.cpp
 * \author     Uliana Filonenko
 * \date       2026/05/19
*/
//----------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <random>

#include "pgr.h"
#include "Structures.h"
#include "Objects.h"
#include "Camera.h"
#include "Lab.h"
#include "Player.h"

Material triangleMaterial;
std::unique_ptr<ObjectTriangle> triangle;

int lastFrameTime = 0;
float deltaTime = 0.0f;
float lastX = 0.0f;
float lastY = 0.0f;
bool isFlashlightActive = true; 
bool isMouseLocked = true;

Material wallMaterial;
Material floorMaterial;
Material ceilingMaterial;
std::unique_ptr<Maze> maze;
std::unique_ptr<ObjectModel> chair;
std::unique_ptr<ObjectModel> table;
std::unique_ptr<ObjectModel> plant;
std::unique_ptr<ObjectModel> lamp;
std::unique_ptr<ObjectModel> comp;
std::unique_ptr<ObjectModel> box;
std::unique_ptr<ObjectModel> dustbin;
std::unique_ptr<ObjectModel> woman;
std::unique_ptr<ObjectModel> ball;
std::unique_ptr<ObjectSkybox> skybox;
std::unique_ptr<ObjectAnomaly> anomaly;


Player player(glm::vec3(1.0f, 1.5f, 3.0f)); 
Camera activeCamera;
int currentCameraMode = 0; // 0 - player, 1 - static 1, 2 -  static2
Camera staticCameras[2];
std::vector<LightData> visibleLamps;

bool keys[256] = { false };

void initializeApplication() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    initializeShaderPrograms();

    staticCameras[0] = Camera(
        glm::vec3(1.0f, 3.0f, 1.0f),  
        glm::normalize(glm::vec3(1.0f, -0.5f, 1.0f)) 
    );

    staticCameras[1] = Camera(
        glm::vec3(7.0f, 3.0f, 7.0f),
        glm::normalize(glm::vec3(-1.0f, -0.8f, -1.0f))
    );

  
    wallMaterial.shader = &shaderProgram;
    ceilingMaterial.shader = &shaderProgram;


    wallMaterial.textureID = pgr::createTexture("data/wallpaper.jpg");
    ceilingMaterial.textureID = pgr::createTexture("data/ceilling.jpg");
    wallMaterial.dirtTextureID = pgr::createTexture("data/dirt2.png");


    floorMaterial.shader = &shaderProgram;
    floorMaterial.textureID = 0;

    chair = std::make_unique<ObjectModel>("data/old_chair/old_chair.obj");
    table = std::make_unique<ObjectModel>("data/old_table/Table.obj");
    plant = std::make_unique<ObjectModel>("data/plant/BushObj.obj");
    lamp = std::make_unique<ObjectModel>("data/lamp/IT-724.obj");
    comp = std::make_unique<ObjectModel>("data/old-computer/Computer1.obj");
    box = std::make_unique<ObjectModel>("data/box/box.obj");
    dustbin = std::make_unique<ObjectModel>("data/dustbin/Dustbin.obj");
    ball = std::make_unique<ObjectModel>("data/ball/pallone calcio2.obj");
    woman = std::make_unique<ObjectModel>("data/cekajici_zena/cekajici_zena.obj"); 

    skybox = std::make_unique<ObjectSkybox>(&wallMaterial);
    anomaly = std::make_unique<ObjectAnomaly>(&wallMaterial, glm::vec3(4.0f, 2.0f, 4.0f));


    maze = std::make_unique<Maze>(&wallMaterial, &floorMaterial, &ceilingMaterial, gameConfig.mazeSeed);
    maze->Update(player.Position, activeCamera.Front, deltaTime);
}


void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int currentFrameTime = glutGet(GLUT_ELAPSED_TIME);

    deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentFrameTime;

    if (keys['w']) player.ProcessKeyboard(FORWARD, deltaTime, maze.get());
    if (keys['s']) player.ProcessKeyboard(BACKWARD, deltaTime, maze.get());
    if (keys['a']) player.ProcessKeyboard(LEFT, deltaTime, maze.get());
    if (keys['d']) player.ProcessKeyboard(RIGHT, deltaTime, maze.get());

    player.AttachCamera(activeCamera);

    glm::mat4 viewMatrix;
    if (currentCameraMode == 0) {
        viewMatrix = activeCamera.GetViewMatrix();
    }
    else {
        viewMatrix = staticCameras[currentCameraMode - 1].GetViewMatrix();
    }

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.0f),
        (float)gameConfig.windowWidth / (float)gameConfig.windowHeight,
        0.1f,
        100.0f
    );

  
    skybox->Draw(viewMatrix, projectionMatrix);
    
    if (maze) {
        maze->Update(player.Position, activeCamera.Front, deltaTime);
        static int frameCounter = 9;
        frameCounter++;
        if (frameCounter % 10 == 0) {
            visibleLamps = maze->GetClosestLamps(player.Position, 20);
        }
    }


    float timeSec = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;


    if (shaderProgram.initialized) {
        glUseProgram(shaderProgram.program);
        glm::vec3 currentCamPos;
        if (currentCameraMode == 0) currentCamPos = activeCamera.Position;
        else currentCamPos = staticCameras[currentCameraMode - 1].Position;

        glUniform1f(shaderProgram.timeLocation, timeSec);
        glUniform3f(shaderProgram.viewPosLocation, activeCamera.Position.x, activeCamera.Position.y, activeCamera.Position.z);

       
        glUniform3f(shaderProgram.spotDirLocation, activeCamera.Front.x, activeCamera.Front.y, activeCamera.Front.z);
        glUniform1f(shaderProgram.spotCutOffLocation, glm::cos(glm::radians(20.0f)));
        glUniform1f(shaderProgram.spotOuterCutOffLocation, glm::cos(glm::radians(30.0f)));
        glUniform1i(shaderProgram.isFlashlightOnLocation, isFlashlightActive ? 1 : 0);

        glUniform1f(shaderProgram.fogDensityLocation, gameConfig.fogDensity);

        glUniform1i(shaderProgram.numPointLightsLocation, 0);
        glUniform1i(shaderProgram.hasTextureLocation, 0);
        glUniform3f(shaderProgram.objectColorLocation, 0.8f, 0.7f, 0.4f);

        glUniform1i(shaderProgram.numPointLightsLocation, visibleLamps.size());

        for (size_t i = 0; i < visibleLamps.size(); ++i) {
            glUniform3f(shaderProgram.pointLightPosLocations[i],
                visibleLamps[i].position.x, visibleLamps[i].position.y, visibleLamps[i].position.z);

            float currentIntensity = visibleLamps[i].baseIntensity;
            if (visibleLamps[i].isFlickering) {
             
                float noise = sin(timeSec * 15.0f + visibleLamps[i].phaseOffset) * sin(timeSec * 27.0f + visibleLamps[i].phaseOffset * 2.0f);

                if (noise < -0.3f) {
                    currentIntensity = 0.05f; 
                }
                else if (noise > 0.8f) {
                    currentIntensity = 2.5f;
                }
            }

            glUniform1f(shaderProgram.pointLightIntensityLocations[i], currentIntensity);
        }
    }

    if (anomaly) {
        anomaly->Update(timeSec); 
        anomaly->Draw(viewMatrix, projectionMatrix);
    }

    if (screenShaderProgram.initialized) {
        glUseProgram(screenShaderProgram.program);
        glUniform1f(screenShaderProgram.timeLocation, timeSec);
    }

    if (maze) maze->Draw(viewMatrix, projectionMatrix);

    glutSwapBuffers();
}

void mouseCallback(int xpos, int ypos) {
    if (!isMouseLocked) return;

    int centerX = gameConfig.windowWidth / 2;
    int centerY = gameConfig.windowHeight / 2;

    //to avoid 8
    if (xpos == centerX && ypos == centerY) return;

    float xoffset = (float)(xpos - centerX);
    float yoffset = (float)(centerY - ypos);

    player.ProcessMouseMovement(xoffset, yoffset);

    //teleport cursor to the center
    glutWarpPointer(centerX, centerY);
}

void keyboardCallback(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 'f' || key == 'F') {
        isFlashlightActive = !isFlashlightActive;
         std::cout << "Flashlight : " << (isFlashlightActive ? "ON" : "OFF") << std::endl;
    }

    if (key == 'n' || key == 'N') {
        player.ToggleNoClip();
        std::cout << "NoClip toggled!" << std::endl;
    }

    if (key == 'r' || key == 'R') {
        gameConfig.load("data/config.txt");
        player.MovementSpeed = gameConfig.playerSpeed;
        player.MouseSensitivity = gameConfig.mouseSensitivity;
        if (maze) maze->renderDistance = gameConfig.renderDistance;
        std::cout << "Config reloaded dynamically!" << std::endl;
    }
    
    if (key == 'c' || key == 'C') {
        currentCameraMode = (currentCameraMode + 1) % 3;
        std::string modes[] = { "Player", "Static CCTV 1", "Static CCTV 2" };
        std::cout << "Active Camera: " << modes[currentCameraMode] << std::endl;
    }

    if (key == 27) {
        if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
            exit(0);
        }
        else {
            isMouseLocked = !isMouseLocked;

            if (isMouseLocked) {
              
                glutSetCursor(GLUT_CURSOR_NONE);
                glutWarpPointer(gameConfig.windowWidth / 2, gameConfig.windowHeight / 2);
            }
            else {
               
                glutSetCursor(GLUT_CURSOR_INHERIT);
            }
        }
    }
}

void keyboardUpCallback(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouseButtonCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (maze) {
            maze->Interact(player.Position, activeCamera.Front);
        }
    }
}

void displayCallback() {
    drawScene();
    glutPostRedisplay();
    glutPassiveMotionFunc(mouseCallback);
}

void reshapeCallback(int w, int h) {
    glViewport(0, 0, w, h);
}

void finalizeApplication() {
    triangle.reset();
}

int main(int argc, char** argv) {
    gameConfig.load("data/config.txt");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GL_MULTISAMPLE);

    glutInitWindowSize(gameConfig.windowWidth, gameConfig.windowHeight); 
    glutCreateWindow("Backrooms Project");
    glutFullScreen();

    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);    
    glutKeyboardUpFunc(keyboardUpCallback); 
    glutMouseFunc(mouseButtonCallback);

    if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR)) {
        std::cerr << "pgr init failed" << std::endl;
        return -1;
    }

    initializeApplication();
    glutWMCloseFunc(finalizeApplication);

    glutMainLoop();

    return 0;
}
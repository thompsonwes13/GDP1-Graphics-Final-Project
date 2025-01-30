//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "GLCommon.h"

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>     // "input output" stream
#include <fstream>      // "file" stream
#include <sstream>      // "string" stream ("string builder" in Java c#, etc.)
#include <string>
#include <vector>
#include <algorithm>

//void ReadPlyModelFromFile(std::string plyFileName);
#include "PlyFileLoaders.h"
#include "Basic_Shader_Manager/cShaderManager.h"
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "sharedThings.h"       // Fly camera
#include "cPhysics.h"
#include "cLightManager.h"
#include <windows.h>    // Includes ALL of windows... MessageBox
#include "cLightHelper/cLightHelper.h"
//
#include "cBasicTextureManager/cBasicTextureManager.h"

std::vector<sMesh*> g_vecMeshesToDraw;

cPhysics* g_pPhysicEngine = NULL;
// This loads the 3D models for drawing, etc.
cVAOManager* g_pMeshManager = NULL;

cBasicTextureManager* g_pTextures = NULL;

//cLightManager* g_pLightManager = NULL;

void AddModelsToScene(cVAOManager* pMeshManager, GLuint shaderProgram);
void AddModelsToScene();

void DrawMesh(sMesh* pCurMesh, GLuint program);

//glm::vec3 cameraEye = glm::vec3(0.0, 0.0, 4.0f);


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

bool isControlDown(GLFWwindow* window);

// Returns NULL if NOT found
sMesh* pFindMeshByFriendlyName(std::string theNameToFind)
{
    for (unsigned int index = 0; index != ::g_vecMeshesToDraw.size(); index++)
    {
        if (::g_vecMeshesToDraw[index]->uniqueFriendlyName == theNameToFind)
        {
            return ::g_vecMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}

void SaveMeshes()
{
    // Save state to file

    if (MessageBox(NULL, L"Would you like to save the mesh locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo << ::g_vecMeshesToDraw[i]->uniqueFriendlyName << "\n";

                meshInfo << "Position - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->positionXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->positionXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->positionXYZ.z << "\n";

                meshInfo << "Rotation - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z << "\n";

                meshInfo << "Scale - ";
                meshInfo << ::g_vecMeshesToDraw[i]->uniformScale << "\n\n";
            }
        }

        meshInfo.close();

        std::cout << "Saved Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadMeshes()
{
    // Load state to file
            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox

    if (MessageBox(NULL, L"Would you like to load the mesh locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            float xPos;
            float yPos;
            float zPos;

            float xRot;
            float yRot;
            float zRot;

            float scale;

            std::string discard;

            if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo >> discard    // Mesh Name
                    >> discard    // "Position"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xPos       // x val
                    >> discard    // "y:"
                    >> yPos       // y val
                    >> discard    // "z:"
                    >> zPos       // z val

                    >> discard    // "Rotation"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xRot       // x val
                    >> discard    // "y:"
                    >> yRot       // y val
                    >> discard    // "z:"
                    >> zRot       // z val

                    >> discard    // "Scale"
                    >> discard    // "-"
                    >> scale;     // scale

                ::g_vecMeshesToDraw[i]->positionXYZ.x = xPos;
                ::g_vecMeshesToDraw[i]->positionXYZ.y = yPos;
                ::g_vecMeshesToDraw[i]->positionXYZ.z = zPos;

                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x = xRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y = yRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z = zRot;

                ::g_vecMeshesToDraw[i]->uniformScale = scale;
            }
        }

        std::cout << "Loaded Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SaveLights()
{
    if (MessageBox(NULL, L"Would you like to save the light locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream lightInfo("lightInfo.txt");
        std::string lightType;
        std::string on_off;

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            if (::g_pLightManager->theLights[i].param1.x == 0.0)
            {
                lightType = "Point";
            }
            else if (::g_pLightManager->theLights[i].param1.x == 1.0)
            {
                lightType = "Spot";
            }
            else if (::g_pLightManager->theLights[i].param1.x == 2.0)
            {
                lightType = "Directional";
            }

            if (::g_pLightManager->theLights[i].param2.x == 0.0)
            {
                on_off = "OFF";
            }
            else if (::g_pLightManager->theLights[i].param2.x == 1.0)
            {
                on_off = "ON";
            }

            lightInfo << "Light " << i + 1 << " " << lightType << " " << on_off << "\n";
            lightInfo << "x: " << ::g_pLightManager->theLights[i].position.x << " ";
            lightInfo << "y: " << ::g_pLightManager->theLights[i].position.y << " ";
            lightInfo << "z: " << ::g_pLightManager->theLights[i].position.z << "\n";
            lightInfo << "Linear: " << ::g_pLightManager->theLights[i].atten.y << " ";
            lightInfo << "Quadratic: " << ::g_pLightManager->theLights[i].atten.z << "\n\n";
        }

        lightInfo.close();

        std::cout << "Saved Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadLights()
{
    if (MessageBox(NULL, L"Would you like to load the light locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream lightInfo("lightInfo.txt");

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            float x;
            float y;
            float z;
            float linear;
            float quadratic;
            std::string lightType;
            std::string on_off;
            std::string discard;

            lightInfo >> discard    // "Light"
                >> discard    // Light #
                >> lightType
                >> on_off
                >> discard    // "x:"
                >> x          // x val
                >> discard    // "y:"
                >> y          // y val
                >> discard    // "z:"
                >> z          // z val
                >> discard    // "Linear:"
                >> linear     // Linear value
                >> discard    // "Quardratic
                >> quadratic;  // Quadratic value

            ::g_pLightManager->theLights[i].position.x = x;
            ::g_pLightManager->theLights[i].position.y = y;
            ::g_pLightManager->theLights[i].position.z = z;
            ::g_pLightManager->theLights[i].atten.y = linear;
            ::g_pLightManager->theLights[i].atten.z = quadratic;

            if (lightType == "Point")
            {
                ::g_pLightManager->theLights[i].param1.x = 0.0f;
            }
            else if (lightType == "Spot")
            {
                ::g_pLightManager->theLights[i].param1.x = 1.0f;
            }
            else if (lightType == "Directional")
            {
                ::g_pLightManager->theLights[i].param1.x = 2.0f;
            }

            if (on_off == "OFF")
            {
                ::g_pLightManager->theLights[i].param2.x = 0.0f;
            }
            else if (on_off == "ON")
            {
                ::g_pLightManager->theLights[i].param2.x = 1.0f;
            }
        }

        std::cout << "Loaded Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SetCameraPosition()
{
    std::ifstream cameraInfo("cameraInfo.txt");

    float x;
    float y;
    float z;
    float turnLR;
    float turnUD;
    std::string discard;

    cameraInfo >> discard // "Camera"
        >> discard        // "x:"
        >> x              // x value
        >> discard        // "y:"
        >> y              // y value
        >> discard        // "z:"
        >> z              // z value
        >> discard        // "Rotation-Left-Right:"
        >> turnLR         // Left Right value
        >> discard        // "Rotation-Up-Down:"
        >> turnUD;        // Up Down value

    ::g_pFlyCamera->setEyeLocation(x, y, z);
    ::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(turnLR));
    ::g_pFlyCamera->pitchUpDown(turnUD);

    std::cout << "Camera Position Loaded!" << std::endl;

    return;
}

void ModelsToVAO(const GLuint program)
{
    std::ifstream loadMeshes("loadMeshes.txt");

    //std::string discard;
    //std::string name;
    std::string path;
    //std::string isWireFrame;

    while (!loadMeshes.eof())
    {
        loadMeshes >> path;           // Path value

        sModelDrawInfo mesh;
        ::g_pMeshManager->LoadModelIntoVAO(path, mesh, program);
        std::cout << mesh.numberOfVertices << " vertices loaded" << std::endl;
    }
}

void DrawMeshes()
{
    std::ifstream drawMeshes("drawMeshes.txt");

    std::string discard;
    std::string name;
    std::string path;
    std::string isWireFrame;
    float transparency;
    std::string texture;
    float texScale;

    bool wireframe;

    float r;
    float g;
    float b;

    while (!drawMeshes.eof())
    {
        drawMeshes >> discard // "Name:"
            >> name           // Name value
            >> discard        // "Path:"
            >> path           // Path value
            >> discard        // "isWireframe"
            >> isWireFrame    // isWireFrame value
            >> discard        // "Colour:"
            >> r              // r value
            >> g              // g value
            >> b              // b value
            >> discard        // "Transparency:"
            >> transparency   // transparency value
            >> discard        // "Texture:"
            >> texture        // texture value
            >> texScale;      // texture scale

        if (drawMeshes.eof())
        {
            break;
        }

        if (isWireFrame == "true")
        {
            wireframe = true;
        }
        else
        {
            wireframe = false;
        }

        sMesh* pMesh = new sMesh();
        pMesh->modelFileName = path;
        pMesh->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
        pMesh->bIsWireframe = wireframe;
        pMesh->uniformScale = 0.1f;
        pMesh->objectColourRGBA = glm::vec4((r / 255.0f), (g / 255.0f), (b / 255.0f), 1.0f);
        pMesh->uniqueFriendlyName = name;
        pMesh->alphaTransparency = transparency;
        pMesh->textures[0] = texture;
        pMesh->textureScale = 1/texScale;
        pMesh->bModulateTexture = false;

        ::g_vecMeshesToDraw.push_back(pMesh);
    }
}

void LoadTextures()
{
    ::g_pTextures->SetBasePath("assets/textures");
    ::g_pTextures->Create2DTextureFromBMPFile("UV_Test_750x750.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Grey_Brick_Wall_Texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_texture_02.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wood_floor_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("ceiling_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("chandelier_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("window_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("window_cutout.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("door_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_light_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("light_switch_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("cabinet_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("grass_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("low_poly_tree_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("dirt_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("vase_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("basketball_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("scratches_texture.bmp");

    // Load the space skybox
    std::string errorString;
    ::g_pTextures->SetBasePath("assets/textures/skyboxes");
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("Space",
        "DarkStormyLeft2048.bmp",
        "DarkStormyRight2048.bmp",
        "DarkStormyUp2048.bmp",
        "DarkStormyDown2048.bmp",
        "DarkStormyFront2048.bmp",
        "DarkStormyBack2048.bmp", true, errorString))
    {
        std::cout << "Loaded space skybox" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load space skybox because: " << errorString << std::endl;
    }

    // Load an "interior" skybox
    ::g_pTextures->SetBasePath("assets/textures/skyboxes");
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("Interior",
        "wall_texture_011.bmp",
        "wall_texture_011.bmp",
        "ceiling_texture_01.bmp",
        "wood_floor_texture_01.bmp",
        "wall_texture_011.bmp",
        "wall_texture_011.bmp", true, errorString))
    {
        std::cout << "Loaded interior skybox" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load interior skybox because: " << errorString << std::endl;
    }

    return;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float CAMERA_MOVE_SPEED = 0.1f;

    // Save the mesh locations, rotations, etc.
    if (mods == GLFW_MOD_SHIFT)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file

            SaveMeshes();

        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state from file

            LoadMeshes();

        }
    }

    // Save the light locations, rotations, etc.
    if (mods == GLFW_MOD_CONTROL)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file

            SaveLights();

        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state from file

            LoadLights();

        }
    }

    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
    {
        // check if you are out of bounds
        if (::g_selectedLightIndex > 0)
        {
            ::g_selectedLightIndex--;
        }
        else
        {
            ::g_selectedLightIndex = cLightManager::NUMBEROFLIGHTS - 1;
        }
    }
    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
    {
        ::g_selectedLightIndex++;
        if (::g_selectedLightIndex >= cLightManager::NUMBEROFLIGHTS)
        {
            ::g_selectedLightIndex = 0;
        }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == 1.0)
        {
            ::g_pLightManager->theLights[g_selectedLightIndex].param2.x = 0.0;
        }
        else if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == 0.0)
        {
            ::g_pLightManager->theLights[g_selectedLightIndex].param2.x = 1.0;
        }
    }

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        //back in meshes
        if (::g_selectedMeshIndex > 0)
        {
            ::g_selectedMeshIndex--;
        }
        else
        {
            g_selectedMeshIndex = g_vecMeshesToDraw.size() - 1;
        }
    }
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        //forward in meshes
        ::g_selectedMeshIndex++;
        if (::g_selectedMeshIndex >= g_vecMeshesToDraw.size())
        {
            ::g_selectedMeshIndex = 0;
        }
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        if (g_bShowDebugSpheres)
        {
            g_bShowDebugSpheres = false;
        }
        else if (!g_bShowDebugSpheres)
        {
            g_bShowDebugSpheres = true;
        }
    }

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        if (::g_vecMeshesToDraw[::g_selectedMeshIndex]->bIsWireframe)
        {
            ::g_vecMeshesToDraw[::g_selectedMeshIndex]->bIsWireframe = false;
        }
        else
        {
            ::g_vecMeshesToDraw[::g_selectedMeshIndex]->bIsWireframe = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (movingWalls)
        {
            movingWalls = false;
        }
        else
        {
            movingWalls = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (g_bShowAxis)
        {
            g_bShowAxis = false;
        }
        else
        {
            g_bShowAxis = true;
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) // Long hall
    {
        g_pFlyCamera->straightenCamera();
        g_pFlyCamera->setEyeLocation(-4, 5, 0);
        g_pFlyCamera->rotateLeftRight_Yaw(942.45);
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) // Short hall
    {
        g_pFlyCamera->straightenCamera();
        g_pFlyCamera->setEyeLocation(90, 5, -54);
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) // Window
    {
        g_pFlyCamera->straightenCamera();
        g_pFlyCamera->setEyeLocation(50, 5, -2);
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS) // Vase
    {
        g_pFlyCamera->straightenCamera();
        g_pFlyCamera->setEyeLocation(36, 4.5, -4.25);
        g_pFlyCamera->rotateLeftRight_Yaw(942.45);
    }

    return;
}

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

glm::vec3 getRandom_vec3(glm::vec3 min, glm::vec3 max)
{
    return glm::vec3(
        getRandomFloat(min.x, max.x),
        getRandomFloat(min.y, max.y),
        getRandomFloat(min.z, max.z));
}

bool isObjectACloserThanObjectB(sMesh* pA, sMesh* pB)
{
    float distanceToA = glm::distance(::g_pFlyCamera->getEyeLocation(), pA->positionXYZ);
    float distanceToB = glm::distance(::g_pFlyCamera->getEyeLocation(), pB->positionXYZ);

    if (distanceToA > distanceToB)
    {
        return true;
    }
    return false;
}

int main(void)
{
    //AABBOctTree();

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Callback for keyboard, but for "typing"
    // Like it captures the press and release and repeat
    glfwSetKeyCallback(window, key_callback);

    // 
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    cShaderManager* pShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "assets/shaders/vertex01.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "assets/shaders/fragment01.glsl";

    if ( ! pShaderManager->createProgramFromFile("shader01",
                                                 vertexShader, fragmentShader))
    {
        std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
    }
    else
    {
        std::cout << "Shader built OK" << std::endl;
    }

    const GLuint program = pShaderManager->getIDFromFriendlyName("shader01");

    glUseProgram(program);

    // Loading the TYPES of models I can draw...

//    cVAOManager* pMeshManager = new cVAOManager();
    ::g_pMeshManager = new cVAOManager();

    ModelsToVAO(program);

    ::g_pPhysicEngine = new cPhysics();

    AddModelsToScene();

    // For triangle meshes, let the physics object "know" about the VAO manager
    ::g_pPhysicEngine->setVAOManager(::g_pMeshManager);


    // This also adds physics objects to the phsyics system
    //AddModelsToScene(::g_pMeshManager, program);
    
    ::g_pFlyCamera = new cBasicFlyCamera();
//    ::g_pFlyCamera->setEyeLocation(glm::vec3(0.0f, 10.0f, 50.0f));
    //::g_pFlyCamera->setEyeLocation(glm::vec3(0.0f, 50.0f, 0.0f));
    // Rotate the camera 180 degrees
    //::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(90.0f));
    //::g_pFlyCamera->pitchUpDown(1000.0f);

    glUseProgram(program);

    // Enable depth buffering (z buffering)
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    glEnable(GL_DEPTH_TEST);

    double currentFrameTime = glfwGetTime();
    double lastFrameTime = glfwGetTime();

    SetCameraPosition();

    // Set up the lights
    ::g_pLightManager = new cLightManager();
    // Called only once
    ::g_pLightManager->loadUniformLocations(program);

    // 1 
    ::g_pLightManager->theLights[0].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[0].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 2 
    ::g_pLightManager->theLights[1].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[1].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 3 
    ::g_pLightManager->theLights[2].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[2].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 4 
    ::g_pLightManager->theLights[3].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[3].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 5 
    ::g_pLightManager->theLights[4].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[4].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 6 
    ::g_pLightManager->theLights[5].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[5].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 7 
    ::g_pLightManager->theLights[6].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[6].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 8 
    ::g_pLightManager->theLights[7].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[7].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 9 
    ::g_pLightManager->theLights[8].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[8].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 10 
    ::g_pLightManager->theLights[9].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[9].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 11
    ::g_pLightManager->theLights[10].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[10].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 12
    ::g_pLightManager->theLights[11].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[11].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 13
    ::g_pLightManager->theLights[12].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[12].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 14
    ::g_pLightManager->theLights[13].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[13].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 15
    ::g_pLightManager->theLights[14].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[14].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 16
    ::g_pLightManager->theLights[15].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[15].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 17
    ::g_pLightManager->theLights[16].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[16].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 18

    ::g_pLightManager->theLights[17].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[17].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    ::g_pLightManager->theLights[17].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[17].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[17].param1.z = 15.0f;   //  z = outer angle

    //19

    ::g_pLightManager->theLights[18].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[18].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    ::g_pLightManager->theLights[18].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[18].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[18].param1.z = 15.0f;   //  z = outer angle

    // 20 Directional
    ::g_pLightManager->theLights[19].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[19].diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    ::g_pLightManager->theLights[19].atten.y = 0.01f;
    ::g_pLightManager->theLights[19].atten.z = 0.001f;

    ::g_pLightManager->theLights[19].param1.x = 2.0f;    // Directional light (see shader)
    ::g_pLightManager->theLights[19].param2.x = 1.0f;    // Turn on (see shader)

    ::g_pLightManager->theLights[19].direction = glm::vec4(-0.25f, -1.0f, -0.25f, 1.0f);
    ::g_pLightManager->theLights[19].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[19].param1.z = 10.0f;   //  z = outer angle

    LoadLights();

    ::g_pTextures = new cBasicTextureManager();

    LoadTextures();

    GLint iMaxCombinedTextureInmageUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iMaxCombinedTextureInmageUnits);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << iMaxCombinedTextureInmageUnits << std::endl;

    // data returns one value, the maximum number of components of the inputs read by the fragment shader, 
    // which must be at least 128.
    GLint iMaxFragmentInputComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &iMaxFragmentInputComponents);
    std::cout << "GL_MAX_FRAGMENT_INPUT_COMPONENTS = " << iMaxFragmentInputComponents << std::endl;
    
    // data returns one value, the maximum number of individual floating - point, integer, or boolean values 
    // that can be held in uniform variable storage for a fragment shader.The value must be at least 1024. 
    GLint iMaxFragmentUniformComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &iMaxFragmentUniformComponents);
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = " << iMaxFragmentUniformComponents << std::endl;
        
    //  Turn on the blend operation
    glEnable(GL_BLEND);
    // Do alpha channel transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    cLightHelper TheLightHelper;

    // Is the default (cull back facing polygons)
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //        glm::mat4 m, p, v, mvp;
        glm::mat4 matProjection = glm::mat4(1.0f);

        matProjection = glm::perspective(0.6f,           // FOV
            ratio,          // Aspect ratio of screen
            0.1f,           // Near plane (as far from the camera as possible)
            1000.0f);       // Far plane (as near to the camera as possible)
//            0.1f,           // Near plane (as far from the camera as possible)
//            1000.0f);       // Far plane (as near to the camera as possible)

        // View or "camera"
        glm::mat4 matView = glm::mat4(1.0f);

        //        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, 4.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        matView = glm::lookAt(::g_pFlyCamera->getEyeLocation(),
            ::g_pFlyCamera->getTargetLocation(),
            upVector);
        //        matView = glm::lookAt( cameraEye,
        //                               cameraTarget,
        //                               upVector);

        const GLint matView_UL = glGetUniformLocation(program, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

        const GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

        // Calculate elapsed time
        // We'll enhance this
        currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // **************************************************************
        // Sky box
        // Move the sky sphere with the camera
        sMesh* pSkySphere = pFindMeshByFriendlyName("SkySphere");
        pSkySphere->positionXYZ = ::g_pFlyCamera->getEyeLocation();

        // Disable backface culling (so BOTH sides are drawn)
        glDisable(GL_CULL_FACE);
        // Don't perform depth buffer testing
        glDisable(GL_DEPTH_TEST);
        // Don't write to the depth buffer when drawing to colour (back) buffer
        //        glDepthMask(GL_FALSE);
        //        glDepthFunc(GL_ALWAYS);// or GL_LESS (default)
        // GL_DEPTH_TEST : do or not do the test against what's already on the depth buffer

        pSkySphere->bIsVisible = true;
        //        pSkySphere->bDoNotLight = true;

        pSkySphere->uniformScale = 1.0f;

        // Space skybox

        // Tell the shader this is the skybox, so use the cube map
        // uniform samplerCube skyBoxTexture;
        // uniform bool bIsSkyBoxObject;
        GLuint bIsSkyBoxObject_UL = glGetUniformLocation(program, "bIsSkyBoxObject");
        glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_TRUE);

        // Set the cube map texture, just like we do with the 2D
        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("Space");
        //        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("SunnyDay");
                // Make sure this is an unused texture unit
        glActiveTexture(GL_TEXTURE0 + 42);
        // *****************************************
        // NOTE: This is a CUBE_MAP, not a 2D
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeSamplerID);
        //        glBindTexture(GL_TEXTURE_2D, cubeSamplerID);
                // *****************************************
        GLint skyBoxTextureSampler_UL = glGetUniformLocation(program, "skyBoxTextureSampler");
        glUniform1i(skyBoxTextureSampler_UL, 42);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

        // Interior skybox

        // Set the cube map texture, just like we do with the 2D
        GLuint interiorCubeSamplerID = ::g_pTextures->getTextureIDFromName("Interior");
        //        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("SunnyDay");
                // Make sure this is an unused texture unit
        glActiveTexture(GL_TEXTURE0 + 43);
        // *****************************************
        // NOTE: This is a CUBE_MAP, not a 2D
        glBindTexture(GL_TEXTURE_CUBE_MAP, interiorCubeSamplerID);
        //        glBindTexture(GL_TEXTURE_2D, cubeSamplerID);
                // *****************************************
        GLint interiorTextureSampler_UL = glGetUniformLocation(program, "interiorTextureSampler");
        glUniform1i(interiorTextureSampler_UL, 43);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here


        DrawMesh(pSkySphere, program);

        pSkySphere->bIsVisible = false;

        glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_FALSE);

        glEnable(GL_CULL_FACE);
        // Enable depth test and write to depth buffer (normal rendering)
        glEnable(GL_DEPTH_TEST);
        //        glDepthMask(GL_FALSE);
        //        glDepthFunc(GL_LESS);
        // **************************************************************

        ::g_pLightManager->updateShaderWithLightInfo();
        // *******************************************************************
        //    ____                       _                      
        //   |  _ \ _ __ __ ___      __ | |    ___   ___  _ __  
        //   | | | | '__/ _` \ \ /\ / / | |   / _ \ / _ \| '_ \ 
        //   | |_| | | | (_| |\ V  V /  | |__| (_) | (_) | |_) |
        //   |____/|_|  \__,_| \_/\_/   |_____\___/ \___/| .__/ 
        //                                               |_|    
        // Draw all the objects
      
        // Split to solid and transparent

        std::vector<sMesh*> solidObjects;
        std::vector<sMesh*> transparentObjects;

        for (sMesh* pCurMesh : g_vecMeshesToDraw)
        {
            if (pCurMesh->textures[0] == "wall_texture_01.bmp")
            {
                pCurMesh->textures[1] = "dirt_texture.bmp";
                pCurMesh->bModulateTexture = true;
                pCurMesh->bTranslateTexture = true;

                if (movingWalls)
                {
                    pCurMesh->sTranslate += 0.001;
                    pCurMesh->tTranslate += 0.001;
                }

                //pCurMesh->blendRatio[0] = 0.5;
                //pCurMesh->blendRatio[1] = 0.5;
            }

            if (pCurMesh->alphaTransparency < 1.0)
            {
                // Transparent
                transparentObjects.push_back(pCurMesh);
            }
            else
            {
                // Solid
                solidObjects.push_back(pCurMesh);
            }
        }

        // Solid
        for (sMesh* pCurMesh : solidObjects)
        {
            if (pCurMesh->uniqueFriendlyName == "Basketball" ||
                pCurMesh->uniqueFriendlyName == "Cabinet1" ||
                pCurMesh->uniqueFriendlyName == "Cabinet2" ||
                pCurMesh->uniqueFriendlyName == "Cabinet3")
            {
                pCurMesh->textures[1] = "scratches_texture.bmp";
                pCurMesh->bModulateTexture = true;

                pCurMesh->blendRatio[0] = 1.0f;
                pCurMesh->blendRatio[1] = 1.0f;
            }

            DrawMesh(pCurMesh, program);
        }

        // Sort transparent objects
        std::sort(transparentObjects.begin(), transparentObjects.end(), isObjectACloserThanObjectB);

        // Transparent
        for (sMesh* pCurMesh : transparentObjects)
        {
            DrawMesh(pCurMesh, program);
        }

        if (g_vecMeshesToDraw[0]->rotationEulerXYZ.y < 360)
        {
            g_vecMeshesToDraw[0]->rotationEulerXYZ.y += 0.005;
        }
        else
        {
            g_vecMeshesToDraw[0]->rotationEulerXYZ.y = 0;
        }

        // *******************************************************************

//       sMesh* pTheGround = pFindMeshByFriendlyName("Ground");
//       if (pTheGround)
//       {
//           pTheGround->blendRatio[0] += 0.001f;
//           pTheGround->blendRatio[1] = 1.0f - pTheGround->blendRatio[0];
//       }

        /*sMesh* pBasketball = pFindMeshByFriendlyName("Basketball");
        if (pBasketball)
        {
            pBasketball->textures[1] = "scratches_texture.bmp";
            pBasketball->bModulateTexture = true;

            pBasketball->blendRatio[0] = 1.0f;
            pBasketball->blendRatio[1] = 1.0f;
        }*/

        // Draw the Axes
        cPhysics::sLine xAxis;
        cPhysics::sLine yAxis;
        cPhysics::sLine zAxis;

        float xOffset;
        float yOffset;
        float zOffset;

        glm::vec4 xColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
        glm::vec4 yColour;
        glm::vec4 zColour = glm::vec4(0.0, 0.0, 1.0, 1.0);

        bool turnX;
        bool turnZ;

        if (::g_bShowAxis)
        {
            yOffset = 0;
            zOffset = 0;

            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 0 &&
                g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y <= 90)
            {
                xColour = glm::vec4 (1.0, 0.0, 0.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y > 90 &&
                     g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 180)
            {
                xColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 180 &&
                     g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 270)
            {
                xColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 270 &&
                     g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 360)
            {
                xColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y == 360)
            {
                xColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
            }

            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y != 0)
            {
                yOffset = g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y;
                while (yOffset > 90)
                {
                    yOffset -= 90;

                    if (yOffset == 90)
                    {
                        yOffset = 0;
                        continue;
                    }
                }

                yOffset = yOffset / 90;
            }
            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z != 0)
            {
                zOffset = g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z;
                while (zOffset > 90)
                {
                    zOffset -= 90;

                    if (zOffset == 90)
                    {
                        zOffset = 0;
                        continue;
                    }
                }

                zOffset = zOffset / 90;
            }

            xAxis.startXYZ = g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ;

            

            xAxis.startXYZ.x -= 3;
            xAxis.startXYZ.z += yOffset * 3;
            xAxis.startXYZ.x += yOffset * 3;
            xAxis.startXYZ.y -= zOffset * 3;
            xAxis.startXYZ.x += zOffset * 3;

            glm::vec3 xAxisDirection;

            if ((g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z > 90 &&
                g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z < 180) ||
                    (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z >= 270 &&
                    g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z < 360))
            {
                //zOffset = -zOffset;
                xAxis.startXYZ.y -= 3;
                xAxis.startXYZ.x += 3;

                xAxis.startXYZ.y += zOffset * 3;
                xAxis.startXYZ.y += zOffset * 3;
                xAxis.startXYZ.x -= yOffset * 3;
                xAxis.startXYZ.x -= yOffset;
                xAxis.startXYZ.z -= yOffset * 3;
                xAxis.startXYZ.z -= yOffset * 3;

                zOffset = 1 - zOffset;
                yOffset = -yOffset;

                if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y > 0)
                {
                    xAxisDirection = glm::vec3(-0.1/(1.0 - yOffset - zOffset), zOffset, -yOffset);
                }
                else
                {
                    xAxisDirection = glm::vec3(-(1.0 - yOffset - zOffset), zOffset, -yOffset);
                }

            }
            else
            {
                xAxisDirection = glm::vec3(1.0 - yOffset - zOffset, zOffset, -yOffset);
            }

            glm::vec3 xBall_location = xAxis.startXYZ;

            while (glm::distance(xAxis.startXYZ, xBall_location) < 6.0f)
            {
                xBall_location += (xAxisDirection * 0.04f);
                DrawDebugSphere(xBall_location, xColour, 0.03f, program);
            }

            xOffset = 0;
            yOffset = 0;

            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 0 &&
                g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y <= 90)
            {
                zColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y > 90 &&
                     g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 180)
            {
                zColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 180 &&
                g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 270)
            {
                zColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y >= 270 &&
                     g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y < 360)
            {
                zColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
            }
            else if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y == 360)
            {
                zColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
            }

            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y != 0)
            {
                yOffset = g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.y;
                while (yOffset > 90)
                {
                    yOffset -= 90;

                    if (yOffset == 90)
                    {
                        yOffset = 0;
                        continue;
                    }
                }

                yOffset = yOffset / 90;
            }
            if (g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z != 0)
            {
                xOffset = g_vecMeshesToDraw[g_selectedMeshIndex]->rotationEulerXYZ.z / 90;
            }

            zAxis.startXYZ = g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ;
            zAxis.startXYZ.z -= 3;
            zAxis.startXYZ.x -= yOffset * 3;
            zAxis.startXYZ.z += yOffset * 3;

            glm::vec3 zBall_location = zAxis.startXYZ;
            glm::vec3 zAxisDirection = glm::vec3(yOffset, 0.0, 1.0 - yOffset);

            while (glm::distance(zAxis.startXYZ, zBall_location) < 6.0f)
            {
                zBall_location += (zAxisDirection * 0.04f);
                DrawDebugSphere(zBall_location, zColour, 0.03f, program);
            }

            yAxis.startXYZ = g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ;
            yAxis.startXYZ.y -= 3;

            glm::vec3 yBall_location = yAxis.startXYZ;
            glm::vec3 yAxisDirection = glm::vec3(0.0, 1.0, 0.0);

            while (glm::distance(yAxis.startXYZ, yBall_location) < 6.0f)
            {
                yBall_location += (yAxisDirection * 0.04f);
                DrawDebugSphere(yBall_location, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.03f, program);
            }

        }

        // **********************************************************************************
        if (::g_bShowDebugSpheres)
        {

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

            const float DEBUG_LIGHT_BRIGHTNESS = 0.3f;

            const float ACCURACY = 0.1f;       // How many units distance
            float distance_75_percent =
                TheLightHelper.calcApproxDistFromAtten(0.75f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(DEBUG_LIGHT_BRIGHTNESS, 0.0f, 0.0f, 1.0f),
                distance_75_percent,
                program);


            float distance_50_percent =
                TheLightHelper.calcApproxDistFromAtten(0.5f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(0.0f, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
                distance_50_percent,
                program);

            float distance_25_percent =
                TheLightHelper.calcApproxDistFromAtten(0.25f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(0.0f, 0.0f, DEBUG_LIGHT_BRIGHTNESS, 1.0f),
                distance_25_percent,
                program);

            float distance_05_percent =
                TheLightHelper.calcApproxDistFromAtten(0.05f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(DEBUG_LIGHT_BRIGHTNESS, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
                distance_05_percent,
                program);

        }
        // **********************************************************************************

        // Physic update and test 
        ::g_pPhysicEngine->StepTick(deltaTime);

        // Handle any collisions
        if (::g_pPhysicEngine->vec_SphereAABB_Collisions.size() > 0 )
        {
            // Yes, there were collisions

            for (unsigned int index = 0; index != ::g_pPhysicEngine->vec_SphereAABB_Collisions.size(); index++)
            {
                cPhysics::sCollision_SphereAABB thisCollisionEvent = ::g_pPhysicEngine->vec_SphereAABB_Collisions[index];

                if (thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y  < 0.0f)
                {
                    // Yes, it's heading down
                    // So reverse the direction of velocity
                    thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y = fabs(thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y);
                }

            }//for (unsigned int index
 
        }//if (::g_pPhysicEngine->vec_SphereAABB_Collisions

        // Point the spot light to the ball
        //sMesh* pBouncy_5_Ball = pFindMeshByFriendlyName("Bouncy_5");
        //if (pBouncy_5_Ball)
        //{
        //    glm::vec3 directionToBal
        //        = pBouncy_5_Ball->positionXYZ - glm::vec3(::g_pLightManager->theLights[1].position);
    
        //    // Normalize to get the direction only
        //    directionToBal = glm::normalize(directionToBal);

        //    // Point the spot light at the bouncy ball
        //    ::g_pLightManager->theLights[1].direction = glm::vec4(directionToBal, 1.0f);
        //}

        // Handle async IO stuff
        handleKeyboardAsync(window);
        handleMouseAsync(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::string lightOn;

        if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == true)
        {
            lightOn = "ON";
        }
        else
        {
            lightOn = "OFF";
        }

        std::string showAxis;

        if (g_bShowAxis)
        {
            showAxis = "TRUE";
        }
        else
        {
            showAxis = "FALSE";
        }

        std::stringstream ssTitle;
        ssTitle << "Camera: "
            << ::g_pFlyCamera->getEyeLocation().x << ", "
            << ::g_pFlyCamera->getEyeLocation().y << ", "
            << ::g_pFlyCamera->getEyeLocation().z
            << "   ";
        ssTitle << "Light[" << g_selectedLightIndex << "]: "
            << lightOn << " "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.x << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.y << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.z
            << "   "
            << "linear: " << ::g_pLightManager->theLights[g_selectedLightIndex].atten.y
            << "   "
            << "quad: " << ::g_pLightManager->theLights[g_selectedLightIndex].atten.z
            << "   ";
        ssTitle << "mesh[" << g_selectedMeshIndex << "] "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->uniqueFriendlyName << "  "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.x << ", "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.y << ", "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.z
            << "  "
            << showAxis;

        glfwSetWindowTitle(window, ssTitle.str().c_str());

    }// End of the draw loop


    // Delete everything
    delete ::g_pFlyCamera;
    delete ::g_pPhysicEngine;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


void AddModelsToScene(cVAOManager* pMeshManager, GLuint program)
{
    {
        sModelDrawInfo galacticaModel;
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Battlestar_Galactica_Res_0_(444,087 faces)_xyz_n_uv (facing +z, up +y).ply",
            galacticaModel, program);
        std::cout << galacticaModel.meshName << ": " << galacticaModel.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo warehouseModel;
        //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Warehouse_xyz_n_uv.ply",
            warehouseModel, program);
        std::cout << warehouseModel.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo tankModel;
        //    pMeshManager->LoadModelIntoVAO("assets/models/Low_Poly_Tank_Model_3D_model.ply", 
        pMeshManager->LoadModelIntoVAO("assets/models/Low_Poly_Tank_Model_3D_model_xyz_n_uv.ply",
            tankModel, program);
        std::cout << tankModel.meshName << " : " << tankModel.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo terrainModel;
        //    pMeshManager->LoadModelIntoVAO("assets/models/Simple_MeshLab_terrain_xyz_only.ply", 
    //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Simple_MeshLab_terrain_xyz_N.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Simple_MeshLab_terrain_xyz_N_uv.ply",
            terrainModel, program);
        std::cout << terrainModel.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo bunnyModel;
        //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/bun_zipper_res2_10x_size_xyz_only.ply",
    //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/bun_zipper_res2_10x_size_xyz_N_only.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/bun_zipper_res2_10x_size_xyz_N_uv.ply",
            bunnyModel, program);
        std::cout << bunnyModel.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo platPlaneDrawInfo;
        //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Flat_Plane_xyz.ply", 
    //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Flat_Plane_xyz_N.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Flat_Plane_xyz_N_uv.ply",
            platPlaneDrawInfo, program);
        std::cout << platPlaneDrawInfo.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo sphereMesh;
        //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz.ply",
        //::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz_N.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_xyz_N_uv.ply",
            sphereMesh, program);
        std::cout << sphereMesh.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
        sModelDrawInfo sphereShadowMesh;
        //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_Flat_Shadow_xyz_N.ply",
        ::g_pMeshManager->LoadModelIntoVAO("assets/models/Sphere_radius_1_Flat_Shadow_xyz_N_uv.ply",
            sphereShadowMesh, program);
        std::cout << sphereShadowMesh.numberOfVertices << " vertices loaded" << std::endl;
    }

    {
//    ____                _            __                   _     
//   |  _ \ ___ _ __   __| | ___ _ __ / / __ ___   ___  ___| |__  
//   | |_) / _ \ '_ \ / _` |/ _ \ '__/ / '_ ` _ \ / _ \/ __| '_ \ 
//   |  _ <  __/ | | | (_| |  __/ | / /| | | | | |  __/\__ \ | | |
//   |_| \_\___|_| |_|\__,_|\___|_|/_/ |_| |_| |_|\___||___/_| |_|
//                                                                
        sMesh* pWarehouse = new sMesh();
//        pWarehouse->modelFileName = "assets/models/Warehouse_xyz_n.ply";
        pWarehouse->modelFileName = "assets/models/Warehouse_xyz_n_uv.ply";
        pWarehouse->positionXYZ = glm::vec3(-200.0f, 5.0f, 0.0f);
        pWarehouse->rotationEulerXYZ.y = -90.0f;
        pWarehouse->objectColourRGBA = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        //pWarehouse->bIsWireframe = true;
        pWarehouse->bOverrideObjectColour = true;
        pWarehouse->uniqueFriendlyName = "Warehouse";
        //
        pWarehouse->textures[0] = "bad_bunny_1920x1080.bmp";

         ::g_vecMeshesToDraw.push_back(pWarehouse);

//    ____  _               _                  _     _           _   
//   |  _ \| |__  _   _ ___(_) ___ ___    ___ | |__ (_) ___  ___| |_ 
//   | |_) | '_ \| | | / __| |/ __/ __|  / _ \| '_ \| |/ _ \/ __| __|
//   |  __/| | | | |_| \__ \ | (__\__ \ | (_) | |_) | |  __/ (__| |_ 
//   |_|   |_| |_|\__, |___/_|\___|___/  \___/|_.__// |\___|\___|\__|
//                |___/                           |__/               
         ::g_pPhysicEngine->addTriangleMesh(
             "assets/models/Warehouse_xyz_n_uv.ply",
             pWarehouse->positionXYZ,
             pWarehouse->rotationEulerXYZ,
             pWarehouse->uniformScale);

    }

    {
        sMesh* pFlatPlane = new sMesh();
        pFlatPlane->modelFileName = "assets/models/Flat_Plane_xyz_N_uv.ply";
        pFlatPlane->positionXYZ = glm::vec3(0.0f, -5.5f, 0.0f);
        pFlatPlane->rotationEulerXYZ.y = 90.0f;
        pFlatPlane->objectColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        pFlatPlane->uniqueFriendlyName = "Ground";
        //
        pFlatPlane->textures[0] = "dua-lipa-promo.bmp";     // 1.0
        pFlatPlane->textures[1] = "Puzzle_parts.bmp";       // 0.0
        pFlatPlane->textures[2] = "shape-element-splattered-texture-stroke_1194-8223.bmp";
        pFlatPlane->textures[3] = "Grey_Brick_Wall_Texture.bmp";

//        pFlatPlane->alphaTransparency = 0.5f;

        pFlatPlane->blendRatio[0] = 0.0f;
        pFlatPlane->blendRatio[1] = 1.0f;


        //
        //        pFlatPlane->bIsWireframe = true;
        //        ::g_myMeshes[::g_NumberOfMeshesToDraw] = pFlatPlane;
        //        ::g_NumberOfMeshesToDraw++;
        ::g_vecMeshesToDraw.push_back(pFlatPlane);


        // Add the "ground" to the physcs
        cPhysics::sAABB* pAABBGround = new cPhysics::sAABB();
        pAABBGround->centreXYZ = pFlatPlane->positionXYZ;
        sModelDrawInfo planeMeshInfo;
        ::g_pMeshManager->FindDrawInfoByModelName(pFlatPlane->modelFileName, planeMeshInfo);

       // Manually enter the AABB info:
        pAABBGround->centreXYZ = glm::vec3(0.0f, 0.0f, 0.0f);   
        // How far from the centre the XYZ min and max are
        // This information is from the mesh we loaded
        // WARNING: We need to be careful about the scale
        pAABBGround->minXYZ.x = -100.0f;
        pAABBGround->maxXYZ.x = 100.0f;

        pAABBGround->minXYZ.z = -100.0f;
        pAABBGround->maxXYZ.z = 100.0f;

        pAABBGround->minXYZ.y = -1.0f;
        pAABBGround->maxXYZ.y = 1.0f;

        // Copy the physics object position from the initial mesh position
        pAABBGround->pPhysicInfo->position = pFlatPlane->positionXYZ;

        // Don't move this ground (skip integration step)
        pAABBGround->pPhysicInfo->bDoesntMove = true;

        pAABBGround->pPhysicInfo->pAssociatedDrawingMeshInstance = pFlatPlane;

        ::g_pPhysicEngine->vecAABBs.push_back(pAABBGround);
    }

    {

        //    ____                _            __                   _     
        //   |  _ \ ___ _ __   __| | ___ _ __ / / __ ___   ___  ___| |__  
        //   | |_) / _ \ '_ \ / _` |/ _ \ '__/ / '_ ` _ \ / _ \/ __| '_ \ 
        //   |  _ <  __/ | | | (_| |  __/ | / /| | | | | |  __/\__ \ | | |
        //   |_| \_\___|_| |_|\__,_|\___|_|/_/ |_| |_| |_|\___||___/_| |_|
        //                                                                
        sMesh* pSphereMesh = new sMesh();
//        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz.ply";
//        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
        pSphereMesh->positionXYZ = glm::vec3(-15.0f, -3.0f, -20.0f);
        //pSphereMesh->bIsWireframe = true;
        pSphereMesh->objectColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        pSphereMesh->uniqueFriendlyName = "Ball";

        //::g_myMeshes[::g_NumberOfMeshesToDraw] = pSphere;
        //::g_NumberOfMeshesToDraw++;
        ::g_vecMeshesToDraw.push_back(pSphereMesh);

        {
            sMesh* pSphereShadowMesh = new sMesh();
//            pSphereShadowMesh->modelFileName = "assets/models/Sphere_radius_1_Flat_Shadow_xyz_N.ply";
            pSphereShadowMesh->modelFileName = "assets/models/Sphere_radius_1_Flat_Shadow_xyz_N_uv.ply";
            pSphereShadowMesh->positionXYZ = pSphereMesh->positionXYZ;
            pSphereShadowMesh->positionXYZ.y = -3.95f;  // JUST above the ground
            pSphereShadowMesh->objectColourRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            pSphereShadowMesh->uniqueFriendlyName = "Ball_Shadow";
            ::g_vecMeshesToDraw.push_back(pSphereShadowMesh);
        }


        //    ____  _               _                  _     _           _   
        //   |  _ \| |__  _   _ ___(_) ___ ___    ___ | |__ (_) ___  ___| |_ 
        //   | |_) | '_ \| | | / __| |/ __/ __|  / _ \| '_ \| |/ _ \/ __| __|
        //   |  __/| | | | |_| \__ \ | (__\__ \ | (_) | |_) | |  __/ (__| |_ 
        //   |_|   |_| |_|\__, |___/_|\___|___/  \___/|_.__// |\___|\___|\__|
        //                |___/                           |__/               
        // Add sphere
        cPhysics::sSphere* pSphereInfo = new cPhysics::sSphere();

        pSphereInfo->centre = glm::vec3(0.0f);  // Sphere's centre (i.e. an offset from the position)

        pSphereInfo->pPhysicInfo->position = pSphereMesh->positionXYZ;
        // HACK: We know this is 1.0 because...?
        // We could also have pulled that information from the mesh info
        pSphereInfo->radius = 1.0f;

        pSphereInfo->pPhysicInfo->velocity.y = 7.5f;
        
        // Set some x velocity
        pSphereInfo->pPhysicInfo->velocity.x = 1.0f;


        pSphereInfo->pPhysicInfo->acceleration.y = -3.0f;
        
        // Associate this drawing mesh to this physics object
        pSphereInfo->pPhysicInfo->pAssociatedDrawingMeshInstance = pSphereMesh;

        ::g_pPhysicEngine->vecSpheres.push_back(pSphereInfo);
    }


    for ( unsigned int ballCount = 0; ballCount != 10; ballCount++ )
    {
        //    ____                _            __                   _     
        //   |  _ \ ___ _ __   __| | ___ _ __ / / __ ___   ___  ___| |__  
        //   | |_) / _ \ '_ \ / _` |/ _ \ '__/ / '_ ` _ \ / _ \/ __| '_ \ 
        //   |  _ <  __/ | | | (_| |  __/ | / /| | | | | |  __/\__ \ | | |
        //   |_| \_\___|_| |_|\__,_|\___|_|/_/ |_| |_| |_|\___||___/_| |_|
        //                                                                
        sMesh* pSphereMesh = new sMesh();
        //        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz.ply";
//        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
        pSphereMesh->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
        pSphereMesh->positionXYZ.x = getRandomFloat(-30.0f, 30.0f);
        pSphereMesh->positionXYZ.z = getRandomFloat(-30.0f, 30.0f);
        pSphereMesh->positionXYZ.y = getRandomFloat(0.0f, 40.0f);
        //pSphereMesh->bIsWireframe = true;
        pSphereMesh->objectColourRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        pSphereMesh->objectColourRGBA.r = getRandomFloat(0.0f, 1.0f);
        pSphereMesh->objectColourRGBA.g = getRandomFloat(0.0f, 1.0f);
        pSphereMesh->objectColourRGBA.b = getRandomFloat(0.0f, 1.0f);
        std::stringstream ssBallName;
        ssBallName << "Bouncy_" << ballCount;
        pSphereMesh->uniqueFriendlyName = ssBallName.str();

        //
        pSphereMesh->textures[0] = "Non-uniform concrete wall 0512-3-1024x1024.bmp";

        ::g_vecMeshesToDraw.push_back(pSphereMesh);

        //    ____  _               _                  _     _           _   
        //   |  _ \| |__  _   _ ___(_) ___ ___    ___ | |__ (_) ___  ___| |_ 
        //   | |_) | '_ \| | | / __| |/ __/ __|  / _ \| '_ \| |/ _ \/ __| __|
        //   |  __/| | | | |_| \__ \ | (__\__ \ | (_) | |_) | |  __/ (__| |_ 
        //   |_|   |_| |_|\__, |___/_|\___|___/  \___/|_.__// |\___|\___|\__|
        //                |___/                           |__/               
        // Add sphere
        cPhysics::sSphere* pSphereInfo = new cPhysics::sSphere();
        pSphereInfo->centre = glm::vec3(0.0f);  // Sphere's centre (i.e. an offset from the position)
        pSphereInfo->pPhysicInfo->position = pSphereMesh->positionXYZ;
        pSphereInfo->radius = 1.0f;
        pSphereInfo->pPhysicInfo->velocity.y = getRandomFloat(2.0f, 10.0f);
        pSphereInfo->pPhysicInfo->velocity.x = getRandomFloat(-5.0f, 5.0f);
        pSphereInfo->pPhysicInfo->velocity.z = getRandomFloat(-5.0f, 5.0f);
        pSphereInfo->pPhysicInfo->acceleration.y = -3.0f;
        pSphereInfo->pPhysicInfo->pAssociatedDrawingMeshInstance = pSphereMesh;
        ::g_pPhysicEngine->vecSpheres.push_back(pSphereInfo);
    }//for ( unsigned int ballCount

    return;
}

void AddModelsToScene(void)
{
    // Load some models to draw

    DrawMeshes();
  
    LoadMeshes();

    for (int i = 0; i < 500; i++)
    {
        float xCoord = getRandomFloat( -50, 250 );
        float yCoord = getRandomFloat(-2, 0);
        float zCoord = getRandomFloat(-100, 200);

        float scale = getRandomFloat(1.0, 3);

        // Place trees
        sMesh* pTree = new sMesh();
        pTree->modelFileName = "assets/models/hall/low_poly_tree_xyz_N_uv.ply";
        pTree->positionXYZ = glm::vec3(xCoord, yCoord, zCoord);
        pTree->uniformScale = scale;
        pTree->objectColourRGBA = glm::vec4(220/255, 188/255, 140/255, 1.0f);
        pTree->uniqueFriendlyName = "Tree" + std::to_string(i);
        pTree->alphaTransparency = 1.0;
        pTree->textureScale = 1.0;
        pTree->textures[0] = "low_poly_tree_texture.bmp";
        pTree->bModulateTexture = false;

        if (!(xCoord < 115 && zCoord < 15))
        {
            ::g_vecMeshesToDraw.push_back(pTree);
        }
    }

    return;
}
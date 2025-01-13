#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <../../../../glm/glm.hpp> //core glm functionality
#include <../../../../glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <../../../../glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <../../../../glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <vector>
#include <random>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

GLint lightCubeModelLoc;
GLint lightCubeViewLoc;
GLint lightCubeProjLoc;

GLint rearLightCubeModelLoc;
GLint rearLightCubeViewLoc;
GLint rearLightCubeProjLoc;

GLint lightPosLoc1;
GLint lightPosLoc2;
GLint lightPosLoc3;
GLint lightPosLoc4;

GLint rearLightOnLoc;
GLint rearLightCubeOnLoc;
GLint rearLightCubePosLoc;

GLint modelShadowLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

bool cameraAnimation = false;
glm::vec3 cameraOffset = glm::vec3(1.992991f, 0.837006f, 0.0f);

// models
//scene
gps::Model3D racetrack;
GLfloat angle;
//car
gps::Model3D car;
glm::vec3 carPos = glm::vec3(-4.8f, 0.2f, 1.9f);//glm::vec3(-0.65f, 4.0f, 135.0f);
GLfloat carAngle = -1.55f;
//light 1
gps::Model3D lightCube1;
glm::vec3 lightCubePos1 = glm::vec3(-5.225997f, 1.389f, -0.0f);
GLfloat lightCubeAngle1 = 0.0f;
//light 2
gps::Model3D lightCube2;
glm::vec3 lightCubePos2 = glm::vec3(0.07f, 1.389f, -7.135848f);
GLfloat lightCubeAngle2 = 1.570796f;
//light 3
gps::Model3D lightCube3;
glm::vec3 lightCubePos3 = glm::vec3(5.054009f, 1.389f, 0.977995f);
GLfloat lightCubeAngle3 = 0.0f;
//light 4
gps::Model3D lightCube4;
glm::vec3 lightCubePos4 = glm::vec3(-0.316f, 1.323f, 7.253839f);
GLfloat lightCubeAngle4 = 1.570796f;
//rear light
gps::Model3D rearLightCube;
glm::vec3 rearLightCubeOffset = glm::vec3(0.992991f, 0.017006f, 0.0f);
GLfloat rearLightCubeAngle = 0.0f;
GLfloat rearLightCubeSize = 0.009008f;
//screen quad
gps::Model3D screenQuad;

// shaders
gps::Shader myBasicShader;
gps::Shader lightCubeShader;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;
gps::Shader rearLightCubeShader;
gps::Shader rainShaderProgram;

//car animation
bool carAnimation = true;
bool rearLightOn = false;

//shadows 
const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

bool showDepthMap = false;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//solid
bool isSolid = false;

//fog and ambient light
GLint fogDensityLoc;
GLfloat fogDensity = 0.03f;
GLint ambientStrengthLoc;
GLint ambientStrengthSkyBoxLoc;
GLfloat ambientStrength = 0.15f;

//rain
bool rainOn = true;
GLint projectionRainLoc;
GLint viewRainLoc;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    myWindow.setWindowDimensions({ width, height });
    glViewport(0, 0, width, height);

    float aspectRatio = (float) width / (float) height;
    projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lightCubeShader.useShaderProgram();
    glUniformMatrix4fv(lightCubeProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

    rearLightCubeShader.useShaderProgram();
    glUniformMatrix4fv(rearLightCubeProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void updateViewMatrixForMovement() {
    //update view matrix
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    lightCubeShader.useShaderProgram();
    glUniformMatrix4fv(lightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));

    rearLightCubeShader.useShaderProgram();
    glUniformMatrix4fv(rearLightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float xlast = 0, ylast = 0;
    static float yaw = 0, pitch = 0;
    static bool firstMove = true;

    float xoffset = 0.0f;
    float yoffset = 0.0f;

    if (firstMove) {
        firstMove = false;
        xlast = xpos;
        ylast = ypos;
    }
    else {
        xoffset = xpos - xlast;
        yoffset = ypos - ylast;
        xlast = xpos;
        ylast = ypos;
    }
    

    float sensitivity = 0.001f;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::max(pitch, -89.0f);
    pitch = std::min(pitch, 89.0f);

    myCamera.rotate(pitch, yaw);

    updateViewMatrixForMovement();
}

void updateMatrixForRotation() {
    // update model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    // update normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    lightCubeShader.useShaderProgram();
    glUniformMatrix4fv(lightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));

    rearLightCubeShader.useShaderProgram();
    glUniformMatrix4fv(rearLightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

bool ok = false;
void moveCar();

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        updateViewMatrixForMovement();
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        updateViewMatrixForMovement();
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        updateViewMatrixForMovement();
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        updateViewMatrixForMovement();
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        updateMatrixForRotation();
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        updateMatrixForRotation();
    }

    static bool keyReleasedT = true;
    if (pressedKeys[GLFW_KEY_T]) {
        static bool isWireframe = false;

        if (keyReleasedT) {
            isWireframe = !isWireframe;
            keyReleasedT = false;

            if (isWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }
    else {
        keyReleasedT = true;
    }

    static bool keyReleasedY = true;
    if (pressedKeys[GLFW_KEY_Y]) {
        if (keyReleasedY) {
            isSolid = !isSolid;
            keyReleasedY = false;

            myBasicShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "solidView"), isSolid ? 1 : 0);

            rearLightCubeShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(rearLightCubeShader.shaderProgram, "solidView"), isSolid ? 1 : 0);

            skyboxShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(skyboxShader.shaderProgram, "solidView"), isSolid ? 1 : 0);
        }
    }
    else {
        keyReleasedY = true;
    }

    static bool keyReleasedF = true;
    if (pressedKeys[GLFW_KEY_F]) {

        if (keyReleasedF) {
            keyReleasedF = false;
            carAnimation = !carAnimation;
        }
    }
    else {
        keyReleasedF = true;
    }

    static bool keyReleasedZ = true;
    if (pressedKeys[GLFW_KEY_Z]) {

        if (keyReleasedZ) {
            keyReleasedZ = false;
            rainOn = !rainOn;
        }
    }
    else {
        keyReleasedZ = true;
    }

    //animatie camera
    static bool keyReleasedV = true;
    if (pressedKeys[GLFW_KEY_V]) {

        if (keyReleasedV) {
            keyReleasedV = false;
            cameraAnimation = !cameraAnimation;
        }
    }
    else {
        keyReleasedV = true;
    }

    //miscare 
    if (pressedKeys[GLFW_KEY_I]) {
        carPos.x += 0.05;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_K]) {
        carPos.x -= 0.05;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_G]) {
        carPos.x += 0.05;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_H]) {
        carPos.x -= 0.05;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_J]) {
        carPos.z += 0.05f;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        carPos.z -= 0.05f;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_U]) {
        carAngle += 0.05f;
        ok = true;
    }

    if (pressedKeys[GLFW_KEY_O]) {
        carAngle -= 0.05f;
        ok = true;
    }

    static bool keyReleasedN = true;
    if (pressedKeys[GLFW_KEY_N]) {

        if (keyReleasedN) {
            keyReleasedN = false;

            moveCar();
        }
    }
    else {
        keyReleasedN = true;
    }



    //update light and fog
    if (pressedKeys[GLFW_KEY_1]) {
        if (ambientStrength > 0.05) {
            ambientStrength -= 0.01;

            myBasicShader.useShaderProgram();
            glUniform1f(ambientStrengthLoc, ambientStrength);

            skyboxShader.useShaderProgram();
            glUniform1f(ambientStrengthSkyBoxLoc, ambientStrength);
        }
    }

    if (pressedKeys[GLFW_KEY_2]) {
        if (ambientStrength < 0.6) {
            ambientStrength += 0.01;

            myBasicShader.useShaderProgram();
            glUniform1f(ambientStrengthLoc, ambientStrength);

            skyboxShader.useShaderProgram();
            glUniform1f(ambientStrengthSkyBoxLoc, ambientStrength);
        }
    }

    if (pressedKeys[GLFW_KEY_3]) {
        if (fogDensity > 0.0) {
            fogDensity -= 0.01;

            myBasicShader.useShaderProgram();
            glUniform1f(fogDensityLoc, fogDensity);
        }
    }

    if (pressedKeys[GLFW_KEY_4]) {
        if (fogDensity < 0.3) {
            fogDensity += 0.01;

            myBasicShader.useShaderProgram();
            glUniform1f(fogDensityLoc, fogDensity);
        }
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    car.LoadModel("models/formula 1/masinabackup.obj");
    racetrack.LoadModel("models/racetrack/CircuitUVsiGarage.obj");
    lightCube1.LoadModel("models/cube/cube.obj");
    lightCube2.LoadModel("models/cube/cube.obj");
    lightCube3.LoadModel("models/cube/cube.obj");
    lightCube4.LoadModel("models/cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    rearLightCube.LoadModel("models/cube/cube.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");

    lightCubeShader.loadShader(
        "shaders/lightCube.vert",
        "shaders/lightCube.frag");

    depthMapShader.loadShader(
        "shaders/depthShader.vert",
        "shaders/depthShader.frag");

    screenQuadShader.loadShader(
        "shaders/screenQuad.vert", 
        "shaders/screenQuad.frag");

    skyboxShader.loadShader(
        "shaders/skyboxShader.vert", 
        "shaders/skyboxShader.frag");

    rearLightCubeShader.loadShader(
        "shaders/rearLightCube.vert",
        "shaders/rearLightCube.frag");

    rainShaderProgram.loadShader(
        "shaders/rain.vert",
        "shaders/rain.frag");
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 20.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.2f, 5.0f, 0.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //set solid on false
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "solidView"), 0);
    glUniform1i(glGetUniformLocation(skyboxShader.shaderProgram, "solidView"), 0);

    //light
    lightPosLoc1 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos1");
    lightPosLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos2");
    lightPosLoc3 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos3");
    lightPosLoc4 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos4");

    rearLightCubePosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "rearLightPos");
    rearLightOnLoc = glGetUniformLocation(myBasicShader.shaderProgram, "rearLightOn");
    glUniform1i(rearLightCubeOnLoc, rearLightOn ? 1 : 0);

    //ambient strenght and fog density
    ambientStrengthLoc = glGetUniformLocation(myBasicShader.shaderProgram, "ambientStrength");
    glUniform1f(ambientStrengthLoc, ambientStrength);
    
    fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, fogDensity);

    //light cube
    lightCubeShader.useShaderProgram();

    lightCubeModelLoc = glGetUniformLocation(lightCubeShader.shaderProgram, "model");

    lightCubeViewLoc = glGetUniformLocation(lightCubeShader.shaderProgram, "view");
    glUniformMatrix4fv(lightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));

    lightCubeProjLoc = glGetUniformLocation(lightCubeShader.shaderProgram, "projection");
    glUniformMatrix4fv(lightCubeProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //rear light cube
    rearLightCubeShader.useShaderProgram();

    rearLightCubeModelLoc = glGetUniformLocation(rearLightCubeShader.shaderProgram, "model");

    rearLightCubeViewLoc = glGetUniformLocation(rearLightCubeShader.shaderProgram, "view");
    glUniformMatrix4fv(lightCubeViewLoc, 1, GL_FALSE, glm::value_ptr(view));

    rearLightCubeProjLoc = glGetUniformLocation(rearLightCubeShader.shaderProgram, "projection");
    glUniformMatrix4fv(lightCubeProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

    rearLightCubeOnLoc = glGetUniformLocation(rearLightCubeShader.shaderProgram, "rearLightOn");
    glUniform1i(rearLightCubeOnLoc, rearLightOn ? 1 : 0);

    glUniform1i(glGetUniformLocation(rearLightCubeShader.shaderProgram, "solidView"), isSolid ? 1 : 0);

    //shadow
    depthMapShader.useShaderProgram();
    modelShadowLoc = glGetUniformLocation(depthMapShader.shaderProgram, "model");

    //skybox
    skyboxShader.useShaderProgram();
    glUniform3fv(glGetUniformLocation(skyboxShader.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "solidView"), 0);

    ambientStrengthSkyBoxLoc = glGetUniformLocation(myBasicShader.shaderProgram, "ambientStrength");
    glUniform1f(ambientStrengthSkyBoxLoc, ambientStrength);

    //rain
    rainShaderProgram.useShaderProgram();
    projectionRainLoc = glGetUniformLocation(rainShaderProgram.shaderProgram, "projection");
    viewRainLoc = glGetUniformLocation(rainShaderProgram.shaderProgram, "view");
}

void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    mySkyBox.Load(faces);
}

struct Point {
    glm::vec3 point;
    float angle;
};

Point points[20]{};
const int intermediatePoints = 20;
int currentIntermediatePoint = 19;
int currentPoint = 17;
const int totalPoints = 18;

glm::vec3 deltaPoint = glm::vec3(0);
float deltaAngle = 0;

void initPoints() {
    points[0].point = glm::vec3(-4.8f, 0.2f, -1.0f);
    points[0].angle = -1.55f;

    points[1].point = glm::vec3(-4.8f, 0.2f, -4.25f);
    points[1].angle = -1.55f;

    points[2].point = glm::vec3(-3.8f, 0.2f, -5.8f);
    points[2].angle = -2.3f;

    points[3].point = glm::vec3(-1.9f, 0.2f, -6.65f);
    points[3].angle = -3.01f;

    points[4].point = glm::vec3(2.36f, 0.2f, -6.715f);
    points[4].angle = -3.13f;

    points[5].point = glm::vec3(4.301f, 0.2f, -5.565f);
    points[5].angle = -3.83f;

    points[6].point = glm::vec3(4.81f, 0.2f, -4.165f);
    points[6].angle = -4.689f;

    points[7].point = glm::vec3(4.21f, 0.2f, -1.615f);
    points[7].angle = -4.94f;

    points[8].point = glm::vec3(4.5f, 0.2f, 0.115f);
    points[8].angle = -4.6395f;

    points[9].point = glm::vec3(5.25f, 0.2f, 3.565f);
    points[9].angle = -4.386f;

    points[10].point = glm::vec3(5.84f, 0.2f, 5.1f);
    points[10].angle = -4.386;

    points[11].point = glm::vec3(5.74f, 0.2f, 6.7f);
    points[11].angle = -4.83;

    points[12].point = glm::vec3(4.24f, 0.2f, 8.05f);
    points[12].angle = -6.086;

    points[13].point = glm::vec3(1.84f, 0.2f, 7.05f);
    points[13].angle = -6.686;

    points[14].point = glm::vec3(-1.85f, 0.2f, 6.35f);
    points[14].angle = -6.43;

    points[15].point = glm::vec3(-3.25f, 0.2f, 5.9f);
    points[15].angle = -6.686;

    points[16].point = glm::vec3(-4.4f, 0.2f, 5.0624f);
    points[16].angle = -7.1;

    points[17].point = glm::vec3(-4.8f, 0.2f, 1.9f);
    points[17].angle = -7.77;
}

//animation
double lastTimeStampCarUpdate = glfwGetTime();
void updateCarPosition() {
    double currentTime = glfwGetTime();
    if (currentTime - lastTimeStampCarUpdate >= 0.01) {
        moveCar();
        lastTimeStampCarUpdate = currentTime;
    }
}

glm::vec3 catmullRomInterpolation(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
        );
}

float interpolateAngle(float a0, float a1, float t) {
    float diff = a1 - a0;
    if (diff > glm::pi<float>()) {
        diff -= glm::two_pi<float>();
    }
    else if (diff < -glm::pi<float>()) {
        diff += glm::two_pi<float>();
    }
    return a0 + diff * t;
}

void moveCar() {
    if (currentIntermediatePoint + 1 == intermediatePoints) {
        currentIntermediatePoint = 0;

        currentPoint = (currentPoint + 1) % totalPoints;
    }

    int p0 = (currentPoint - 1 + totalPoints) % totalPoints;
    int p1 = currentPoint;
    int p2 = (currentPoint + 1) % totalPoints;
    int p3 = (currentPoint + 2) % totalPoints;

    float t = (float)currentIntermediatePoint / (float)intermediatePoints;

    carPos = catmullRomInterpolation(points[p0].point, points[p1].point, points[p2].point, points[p3].point, t);
    carAngle = interpolateAngle(points[p1].angle, points[p2].angle, t);

    currentIntermediatePoint++;
}

void renderCar(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    glm::mat4 aux = model;

    aux = glm::translate(aux, carPos);

    aux = glm::rotate(aux, carAngle, glm::vec3(0, 1, 0));

    aux = glm::scale(aux, glm::vec3(0.065f, 0.065f, 0.065f));

    if (!depthPass) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aux));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    else {
        glUniformMatrix4fv(modelShadowLoc, 1, GL_FALSE, glm::value_ptr(aux));
    }

    car.Draw(shader);
}

void renderRacetrack(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    glm::mat4 aux = model;
    aux = glm::scale(aux, glm::vec3(0.2f, 0.2f, 0.2f));
    
    if (!depthPass) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aux));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    else {
        glUniformMatrix4fv(modelShadowLoc, 1, GL_FALSE, glm::value_ptr(aux));
    }

    racetrack.Draw(shader);
}

void renderLightCube1(gps::Shader shaderLightCube, gps::Shader shader) {
    shaderLightCube.useShaderProgram();

    glm::mat4 aux = model;
    
    aux = glm::translate(aux, lightCubePos1);
    aux = glm::rotate(aux, lightCubeAngle1, glm::vec3(0, 1, 0));
    aux = glm::scale(aux, glm::vec3(0.065f, 0.012f, 0.035f));

    glUniformMatrix4fv(lightCubeModelLoc, 1, GL_FALSE, glm::value_ptr(aux));

    lightCube1.Draw(shaderLightCube);

    shader.useShaderProgram();
    glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightCubePos1));
}

void renderLightCube2(gps::Shader shaderLightCube, gps::Shader shader) {
    shaderLightCube.useShaderProgram();

    glm::mat4 aux = model;

    aux = glm::translate(aux, lightCubePos2);
    aux = glm::rotate(aux, lightCubeAngle2, glm::vec3(0, 1, 0));
    aux = glm::scale(aux, glm::vec3(0.065f, 0.012f, 0.035f));

    glUniformMatrix4fv(lightCubeModelLoc, 1, GL_FALSE, glm::value_ptr(aux));

    lightCube1.Draw(shaderLightCube);

    shader.useShaderProgram();
    glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightCubePos2));
}

void renderLightCube3(gps::Shader shaderLightCube, gps::Shader shader) {
    shaderLightCube.useShaderProgram();

    glm::mat4 aux = model;

    aux = glm::translate(aux, lightCubePos3);
    aux = glm::rotate(aux, lightCubeAngle3, glm::vec3(0, 1, 0));
    aux = glm::scale(aux, glm::vec3(0.065f, 0.012f, 0.035f));

    glUniformMatrix4fv(lightCubeModelLoc, 1, GL_FALSE, glm::value_ptr(aux));

    lightCube3.Draw(shaderLightCube);

    shader.useShaderProgram();
    glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightCubePos3));
}

void renderLightCube4(gps::Shader shaderLightCube, gps::Shader shader) {
    shaderLightCube.useShaderProgram();

    glm::mat4 aux = model;

    aux = glm::translate(aux, lightCubePos4);
    aux = glm::rotate(aux, lightCubeAngle4, glm::vec3(0, 1, 0));
    aux = glm::scale(aux, glm::vec3(0.065f, 0.012f, 0.035f));

    glUniformMatrix4fv(lightCubeModelLoc, 1, GL_FALSE, glm::value_ptr(aux));

    lightCube1.Draw(shaderLightCube);

    shader.useShaderProgram();
    glUniform3fv(lightPosLoc4, 1, glm::value_ptr(lightCubePos4));
}

double lastTimeStampLightUpdate = glfwGetTime();
void updateLight() {
    double currentTime = glfwGetTime();
    if (currentTime - lastTimeStampLightUpdate >= 0.5) {
        rearLightOn = !rearLightOn;

        lastTimeStampLightUpdate = currentTime;

        myBasicShader.useShaderProgram();
        glUniform1i(rearLightOnLoc, rearLightOn ? 1 : 0);

        rearLightCubeShader.useShaderProgram();
        glUniform1i(rearLightCubeOnLoc, rearLightOn ? 1 : 0);
    }
}

void renderRearLightCube(gps::Shader shaderLightCube, gps::Shader shader) {
    glm::vec4 transformedPos = glm::vec4(rearLightCubeOffset, 1.0f);
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, carPos);
    transformMatrix = glm::rotate(transformMatrix, carAngle, glm::vec3(0, 1, 0));
    transformedPos = transformMatrix * transformedPos;
    glm::vec3 lightPos = glm::vec3(transformedPos);

    shaderLightCube.useShaderProgram();

    glm::mat4 aux = model;

    aux = glm::translate(aux, carPos);
    aux = glm::rotate(aux, carAngle, glm::vec3(0, 1, 0));
    aux = glm::translate(aux, rearLightCubeOffset);
    aux = glm::scale(aux, glm::vec3(rearLightCubeSize, rearLightCubeSize, rearLightCubeSize));

    glUniformMatrix4fv(lightCubeModelLoc, 1, GL_FALSE, glm::value_ptr(aux));

    lightCube1.Draw(shaderLightCube);

    shader.useShaderProgram();
    glUniform3fv(rearLightCubePosLoc, 1, glm::value_ptr(lightPos));
}

void renderSkyBox(gps::Shader shader) {
    shader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);
}

void animations() {
    if (carAnimation) {
        updateCarPosition();
    }

    updateLight();
}

//rain
struct Raindrop {
    float x, y, z;
    float speed;
};

std::vector<Raindrop> raindrops;
GLuint vao, vbo;

float randomFloat(float min, float max) {
    return min + (rand() % 1000) * (max - min) / 1000.0f;
}

void initRain(int numDrops) {
    for (int i = 0; i < numDrops; i++) {
        raindrops.push_back({
            randomFloat(-10.0f, 10.0f),  // x
            randomFloat(0.0f, 20.0f),    // y
            randomFloat(-10.0f, 10.0f),  // z
            randomFloat(0.1f, 0.2f)
            });
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void updateRain() {
    for (auto& drop : raindrops) {
        drop.y -= drop.speed;
        if (drop.y < 0.0f) {
            drop.y = 20.0f;
            drop.speed = randomFloat(0.1f, 0.2f);
        }
    }

    // Update buffer with new positions
    std::vector<float> positions;
    for (const auto& drop : raindrops) {
        positions.push_back(drop.x);
        positions.push_back(drop.y);
        positions.push_back(drop.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
}

void drawRain() {
    rainShaderProgram.useShaderProgram();

    glUniformMatrix4fv(projectionRainLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewRainLoc, 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glDrawArrays(GL_POINTS, 0, raindrops.size());

    glDisable(GL_BLEND);
}

void cleanupRain() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

//camera
void moveCamera() {
    glm::vec4 transformedPos = glm::vec4(cameraOffset, 1.0f);
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, carPos);
    transformMatrix = glm::rotate(transformMatrix, carAngle, glm::vec3(0, 1, 0));
    transformedPos = transformMatrix * transformedPos;

    myCamera.setCamera(transformedPos, carPos);
    updateViewMatrixForMovement();
}

//shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;
void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO 
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO 
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    // aplica rotatia
    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    const GLfloat near_plane = 0.1f, far_plane = 30.0f;
    glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void renderScene() {

    animations();

    glm::mat4 lightSpaceTrMatrix = computeLightSpaceTrMatrix();

    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderRacetrack(depthMapShader, true);
    renderCar(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render depth map on screen - toggled with the M key

    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        renderRacetrack(myBasicShader, false);
        renderCar(myBasicShader, false);

        //draw a white cube around the light
        renderLightCube1(lightCubeShader, myBasicShader);
        renderLightCube2(lightCubeShader, myBasicShader);
        renderLightCube3(lightCubeShader, myBasicShader);
        renderLightCube4(lightCubeShader, myBasicShader);

        renderRearLightCube(rearLightCubeShader, myBasicShader);

        renderSkyBox(skyboxShader);

        if (rainOn) {
            updateRain();
            drawRain();
        }

        if (cameraAnimation) {
            moveCamera();
        }
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();

    //my initializations
    initPoints();
    initFBO();
    initSkyBox();
    initRain(5000);

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

    cleanupRain();
	cleanup();

    return EXIT_SUCCESS;
}

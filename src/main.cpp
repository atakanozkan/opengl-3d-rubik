#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <sstream>
#include "cube.h"
#include "shader.h"
#include "rubiks2x2.cpp"

float fov = glm::radians(45.0f);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
double lastX = 0.0, lastY = 0.0;

float cameraTheta = M_PI / 4;
float cameraPhi = M_PI / 4;
float cameraRadius = 11.0f;

int surfaceType = 0;
const char* vertexShaderPath = "src/vertex_shader.glsl";
const char* fragmentShaderPath = "src/fragment_shader.glsl";

Rubiks2x2 rubik;
std::vector<int> faceCubes;
float currentRotation = 0.0f;
float targetRotation = 0.0f;
bool isRotating = false;
glm::vec3 rotationAxis(0, 0, 1);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void updateCubePositions();
glm::vec3 calculateCentroid(const std::vector<int>& indices);
void updateCubePositionsAfterRotation();
std::vector<glm::vec3> calculateRotatedPositions(const std::vector<int>& faceCubes, const glm::vec3& rotationAxis, float rotationAngle, const glm::vec3& centroid);
void snapCubesToGrid();
std::string vec3_to_string(const glm::vec3& vec);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rubik's Cube Simulator", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glfwSetTime(0);
    glfwSetKeyCallback(window, keyCallback);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    Shader ourShader(vertexShaderPath, fragmentShaderPath);
    
    rubik.initialize();
    std::vector<Cube> cubes(8, Cube());
    rubik.assignInitialCubes(cubes);

    glm::mat4 projection = glm::perspective(fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 cameraPos = glm::vec3(
            cameraRadius * sin(cameraPhi) * cos(cameraTheta),
            cameraRadius * cos(cameraPhi),
            cameraRadius * sin(cameraPhi) * sin(cameraTheta)
        );
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        for (int i = 0; i < rubik.cubes.size(); ++i) {
            rubik.cubes[i].render(ourShader);
        }

        if (currentRotation != targetRotation) {
            currentRotation += (targetRotation - currentRotation) * 0.1f;
        }

        if (isRotating) {
            updateCubePositions();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool keyPressed= false;
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_H:
                std::cout << "CONTROLS :\n"
                          << "Press 1 - 90 degree Rotation on Top surface.\n"
                          << "Press 2 - 90 degree Rotation on Bottom surface.\n"
                          << "Press 3 - 90 degree Rotation on Font surface.\n"
                          << "Press 4 - 90 degree Rotation on Back surface.\n"
                          << "Press 5 - 90 degree Rotation on Left surface.\n"
                          << "Press 6 - 90 degree Rotation on Right surface.\n";
                break;
            case GLFW_KEY_1: // Top face
                faceCubes = {0, 1, 3, 2};
                surfaceType = 1;
                rotationAxis = glm::vec3(0, 0, 1);
                keyPressed= true;
                break;
            case GLFW_KEY_2: // Bottom face
                faceCubes = {5, 4, 6, 7};
                surfaceType = 2;
                rotationAxis = glm::vec3(0, 0, -1);
                keyPressed= true;
                break;
            case GLFW_KEY_3: // Front face
                faceCubes = {4, 5, 1, 0};
                surfaceType = 3;
                rotationAxis = glm::vec3(0, -1, 0);
                keyPressed= true;
                break;
            case GLFW_KEY_4: // Back face
                faceCubes = {2, 3, 7, 6};
                surfaceType = 4;
                rotationAxis = glm::vec3(0, 1, 0);
                keyPressed= true;
                break;
            case GLFW_KEY_5: // Left face
                faceCubes = {4 ,0 , 2 ,6};
                surfaceType = 5;
                rotationAxis = glm::vec3(-1, 0, 0);
                keyPressed= true;
                break;
            case GLFW_KEY_6: // Right face
                faceCubes = {1, 5, 7, 3};
                surfaceType = 6;
                rotationAxis = glm::vec3(1, 0, 0);
                keyPressed= true;
                break;
        }
        if (!isRotating && keyPressed) {
            targetRotation = glm::radians(90.0f);
            currentRotation = 0;
            isRotating = true;
            keyPressed  =false;
        }
    }
}




void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &lastX, &lastY);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        float deltaX = (float)(xpos - lastX);
        float deltaY = (float)(ypos - lastY);
        cameraTheta -= deltaX * 0.003f;
        cameraPhi = glm::clamp(cameraPhi - deltaY * 0.003f, 0.1f, glm::pi<float>() - 0.1f);
        lastX = xpos;
        lastY = ypos;
    }
}

void updateCubePositions() {
    if (isRotating) {
        float rotationStep = glm::radians(90.0f);
        currentRotation += rotationStep;

        if (currentRotation >= glm::radians(90.0f)) {
            currentRotation = 0;
            isRotating = false;
            snapCubesToGrid();

            glm::vec3 centroid = calculateCentroid(faceCubes);
            std::vector<glm::vec3> newPositions = calculateRotatedPositions(faceCubes, rotationAxis, rotationStep, centroid);

            for (size_t i = 0; i < faceCubes.size(); ++i) {
                rubik.cubes[faceCubes[i]].setPosition(newPositions[i]);
                rubik.cubes[faceCubes[i]].updateModelMatrix();
            }

            updateCubePositionsAfterRotation();
        }
    }
}

glm::vec3 calculateCentroid(const std::vector<int>& indices) {
    glm::vec3 centroid(0.0f, 0.0f, 0.0f);
    for (int idx : indices) {
        centroid += rubik.cubes[idx].position;
    }
    centroid /= float(indices.size());
    return centroid;
}


std::vector<glm::vec3> calculateRotatedPositions(const std::vector<int>& faceCubes, const glm::vec3& rotationAxis, float rotationAngle, const glm::vec3& centroid) {
    std::vector<glm::vec3> newPositions;
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

    for (int idx : faceCubes) {
        glm::vec3 localPos = rubik.cubes[idx].position - centroid;
        glm::vec3 rotatedPos = centroid + glm::vec3(rotationMatrix * glm::vec4(localPos, 1.0f));
        newPositions.push_back(rotatedPos);
    }

    return newPositions;
}

void updateCubePositionsAfterRotation() {
    if (!isRotating && currentRotation == 0) {
        rubik.updateCubeOrder(faceCubes, rotationAxis, surfaceType);
    }
}

std::string vec3_to_string(const glm::vec3& vec) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out.str();
}

void snapCubesToGrid() {
    for (auto& cube : rubik.cubes) {
        cube.position.x = std::round(cube.position.x * 2) / 2.0f;
        cube.position.y = std::round(cube.position.y * 2) / 2.0f;
        cube.position.z = std::round(cube.position.z * 2) / 2.0f;
        cube.updateModelMatrix();
    }
}

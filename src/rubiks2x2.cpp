#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include "cube.h"

class Rubiks2x2 {
public:
    std::vector<glm::vec3> initialPositions;
    std::vector<Cube> cubes;

    Rubiks2x2(){

        cubes.reserve(8);
        
        initialize();
    }
    
    void initialize() {
        initialPositions = {
            glm::vec3(-0.5f, -0.5f, 0.5f), // 0
            glm::vec3(0.5f, -0.5f, 0.5f),  // 1
            glm::vec3(-0.5f, 0.5f, 0.5f),  // 2
            glm::vec3(0.5f, 0.5f, 0.5f),   // 3
            glm::vec3(-0.5f, -0.5f, -0.5f),// 4
            glm::vec3(0.5f, -0.5f, -0.5f), // 5
            glm::vec3(-0.5f, 0.5f, -0.5f), // 6
            glm::vec3(0.5f, 0.5f, -0.5f)   // 7
        };
    }

    void assignInitialCubes(const std::vector<Cube>& givenCubes) {
        if (givenCubes.size() != initialPositions.size()) {
            std::cout << "NO MATCH" << std::endl;
            return;
        }
        cubes = givenCubes;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        std::vector<glm::vec3> colorOptions = {
            glm::vec3(1.0f, 0.0f, 0.0f), // Red
            glm::vec3(0.0f, 1.0f, 0.0f), // Green
            glm::vec3(0.0f, 0.0f, 1.0f), // Blue
            glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        };

        std::shuffle(colorOptions.begin(), colorOptions.end(), gen);

        for (size_t i = 0; i < cubes.size(); ++i) {
            cubes[i].setPosition(initialPositions[i]);
            int colorIndex = i % 8;
            cubes[i].setColor(colorOptions[colorIndex]);
        }
        cubes[0].setTag("A0");
        cubes[1].setTag("A1");
        cubes[2].setTag("A2");
        cubes[3].setTag("A3");
        cubes[4].setTag("A4");
        cubes[5].setTag("A5");
        cubes[6].setTag("A6");
        cubes[7].setTag("A7");
    }

    void updateCubeOrder(const std::vector<int>& faceCubes, const glm::vec3& axis, int surfaceType) {
        std::vector<Cube> tempCubes(4);

        if (surfaceType > 0) {
            tempCubes[0] = cubes[faceCubes[3]];
            tempCubes[1] = cubes[faceCubes[0]];
            tempCubes[2] = cubes[faceCubes[1]];
            tempCubes[3] = cubes[faceCubes[2]];
        }

        for (int i = 0; i < 4; ++i) {
            cubes[faceCubes[i]] = tempCubes[i];
        }

    }
};


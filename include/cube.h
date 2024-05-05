#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class Cube {
public:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 color;
    glm::mat4 modelMatrix;
    std::string tag;
    unsigned int VAO, VBO, EBO;

    Cube();
    void setupMesh();
    void setPosition(const glm::vec3& pos);
    void updateModelMatrix();
    void setColor(const glm::vec3& col);
    void setTag(const std::string& newTag);
    void render(Shader& shader);
};

#endif // CUBE_H

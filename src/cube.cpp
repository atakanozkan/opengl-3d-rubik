#include "cube.h"
#include "shader.h"


/**
 In the constructor, it defines vertices of the cube here in total of 6 (cube walls) and setup the mesh for cube object.
 */
Cube::Cube() {

    vertices = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, // Back
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, // Front
        -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, // Top
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, // Bottom
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, // Right
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f  // Left
    };
    
    indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    setupMesh();
}



/**
 This function sets up the mesh for the cube. It generates vertex array, vertex buffer, and element buffer objects,
 It stores their IDs in VAO, VBO, and EBO. It binds and fills the vertex buffer with vertex data and element buffer.
 */
void Cube::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Cube::setColor(const glm::vec3& col) {
    color = col; // Set color
}

void Cube::setTag(const std::string& newTag) {
    tag = newTag;
}

 void Cube::setPosition(const glm::vec3& pos) {
     position = pos;
     updateModelMatrix();
 }

void Cube::updateModelMatrix() {
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    if (rotation.x != 0) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    if (rotation.y != 0) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    if (rotation.z != 0) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
}

/**
 This function binds vertex array object and draw elements
 */

void Cube::render(Shader& shader) {
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", modelMatrix);
    shader.setVec4("chosenColor", glm::vec4(color, 1.0f));
    shader.setInt("useUniformColor", GL_TRUE);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

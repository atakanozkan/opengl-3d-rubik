#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;
    
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif

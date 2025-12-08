#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader() = default;
    ~Shader(); // Destrutor

    bool loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath);
    void use() const;

    // Utilitários de Uniforms
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setInt(const std::string &name, int value) const;
    void setBool(const std::string &name, bool value) const;
 
    void setFloat(const std::string &name, float value) const;
};
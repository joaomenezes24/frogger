#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader() = default;
    ~Shader();
    bool loadFromFiles(const std::string &vertPath, const std::string &fragPath);
    void use() const;
    unsigned int id() const { return ID; }

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &v) const;
    void setInt(const std::string &name, int value) const;
private:
    unsigned int ID = 0;
};
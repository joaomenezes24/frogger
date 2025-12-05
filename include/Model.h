#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Mesh {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int indexCount = 0;
};

class Model {
public:
    Model() = default;
    explicit Model(const std::string &path);
    ~Model();

    bool load(const std::string &path);
    void draw() const;

private:
    std::vector<Mesh> meshes;
};

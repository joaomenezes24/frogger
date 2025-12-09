#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <vector>

class BoundaryLines
{
public:
    BoundaryLines() = default;
    ~BoundaryLines();

    void init(float xPosition, float startZ, float endZ, float dashLength = 2.0f, float gapLength = 1.0f);
    void render();

    unsigned int getVAO() const { return VAO; }

private:
    unsigned int VAO = 0, VBO = 0;
    int vertexCount = 0;

    void setupMesh(const std::vector<float>& vertices);
};
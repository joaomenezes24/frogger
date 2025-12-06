#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Renderer; // forward declaration

class Ground {
public:
    Ground();
    ~Ground();
    
    void init(int numLanes = 5);  // Número de faixas configurável
    void render(Renderer& renderer);
    
    int getNumLanes() const { return numLanes; }
    
private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    int numLanes;
    Shader groundShader;  // Shader específico para o chão
    
    void createGroundMesh();
};
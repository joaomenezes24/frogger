#include "BoundaryLines.h"
#include <cmath>

BoundaryLines::~BoundaryLines()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void BoundaryLines::init(float xPosition, float startZ, float endZ, float dashLength, float gapLength)
{
    std::vector<float> vertices;
    
    float y = 0.08f; // Altura da linha (acima do chão para evitar z-fighting)
    float currentZ = startZ;
    float totalDistance = std::abs(endZ - startZ);
    float direction = (endZ > startZ) ? 1.0f : -1.0f;
    
    // Criar linha tracejada
    while (std::abs(currentZ - startZ) < totalDistance)
    {
        float nextZ = currentZ + (dashLength * direction);
        
        // Limitar ao endZ
        if ((direction > 0 && nextZ > endZ) || (direction < 0 && nextZ < endZ))
            nextZ = endZ;
        
        // Criar um segmento de linha como dois triângulos (quad fino)
        float width = 0.15f; // Largura da linha
        
        // Quad com 6 vértices (2 triângulos)
        // Formato: position(3) + normal(3) + texCoord(2) + color(3) = 11 floats
        
        // Triângulo 1
        // Vértice 1: canto inferior esquerdo
        vertices.push_back(xPosition - width);  // x
        vertices.push_back(y);                   // y
        vertices.push_back(currentZ);            // z
        vertices.push_back(0.0f);                // nx
        vertices.push_back(1.0f);                // ny
        vertices.push_back(0.0f);                // nz
        vertices.push_back(0.0f);                // u
        vertices.push_back(0.0f);                // v
        vertices.push_back(1.0f);                // r (amarelo)
        vertices.push_back(1.0f);                // g
        vertices.push_back(0.0f);                // b
        
        // Vértice 2: canto inferior direito
        vertices.push_back(xPosition + width);
        vertices.push_back(y);
        vertices.push_back(currentZ);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Vértice 3: canto superior esquerdo
        vertices.push_back(xPosition - width);
        vertices.push_back(y);
        vertices.push_back(nextZ);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Triângulo 2
        // Vértice 4: canto superior esquerdo (repetido)
        vertices.push_back(xPosition - width);
        vertices.push_back(y);
        vertices.push_back(nextZ);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Vértice 5: canto inferior direito (repetido)
        vertices.push_back(xPosition + width);
        vertices.push_back(y);
        vertices.push_back(currentZ);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Vértice 6: canto superior direito
        vertices.push_back(xPosition + width);
        vertices.push_back(y);
        vertices.push_back(nextZ);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Pular o gap
        currentZ = nextZ + (gapLength * direction);
        
        if (std::abs(currentZ - startZ) >= totalDistance)
            break;
    }
    
    vertexCount = vertices.size() / 11; // 11 floats por vértice
    setupMesh(vertices);
}

void BoundaryLines::setupMesh(const std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Layout compatível com o shader basic.vert
    // location 0: position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // location 1: normal (nx, ny, nz)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // location 2: texCoord (u, v)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // location 3: color (r, g, b)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
}

void BoundaryLines::render()
{
    if (VAO == 0) return;
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
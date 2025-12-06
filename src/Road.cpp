#include "Road.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Road::Road(int numLanes, float laneWidth, float roadLength) 
    : numLanes(numLanes), laneWidth(laneWidth), roadLength(roadLength),
      roadVAO(0), roadVBO(0), linesVAO(0), linesVBO(0),
      sidewalkVAO(0), sidewalkVBO(0),
      roadVertexCount(0), linesVertexCount(0), sidewalkVertexCount(0) {
}

Road::~Road() {
    if (roadVAO) glDeleteVertexArrays(1, &roadVAO);
    if (roadVBO) glDeleteBuffers(1, &roadVBO);
    if (linesVAO) glDeleteVertexArrays(1, &linesVAO);
    if (linesVBO) glDeleteBuffers(1, &linesVBO);
    if (sidewalkVAO) glDeleteVertexArrays(1, &sidewalkVAO);
    if (sidewalkVBO) glDeleteBuffers(1, &sidewalkVBO);
}

void Road::init() {
    createRoadMesh();
    createLaneLinesMesh();
    createSidewalkMesh();
}

float Road::getLaneZ(int lane) const {
    // Converte índice da faixa para posição Z centralizada
    return lane * laneWidth;
}

float Road::getSidewalkZ() const {
    // Retorna a posição Z da calçada (após a última faixa)
    float lastLaneZ = (numLanes / 2) * laneWidth;
    return lastLaneZ + laneWidth / 2.0f + sidewalkWidth / 2.0f;
}

void Road::createRoadMesh() {
    float totalWidth = numLanes * laneWidth;
    float halfWidth = totalWidth / 2.0f;
    float halfLength = roadLength / 2.0f;
    
    float roadY = -0.28f;
    
    std::vector<float> vertices = {
        // Posição                          Normal           Cor (cinza escuro)
        -halfLength, roadY, -halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
         halfLength, roadY, -halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
         halfLength, roadY,  halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
        
        -halfLength, roadY, -halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
         halfLength, roadY,  halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
        -halfLength, roadY,  halfWidth,     0.0f, 1.0f, 0.0f,   0.3f, 0.3f, 0.3f,
    };
    
    roadVertexCount = 6;
    
    glGenVertexArrays(1, &roadVAO);
    glGenBuffers(1, &roadVBO);
    
    glBindVertexArray(roadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Road::createSidewalkMesh() {
    float halfLength = roadLength / 2.0f;
    float sidewalkY = -0.25f;  // Um pouco acima da estrada
    
    // Calçada fica após a última faixa (Z positivo)
    float lastLaneZ = (numLanes / 2) * laneWidth;
    float sidewalkZStart = lastLaneZ + laneWidth / 2.0f;
    float sidewalkZEnd = sidewalkZStart + sidewalkWidth;
    
    // Cor da calçada (cinza mais claro que a estrada)
    std::vector<float> vertices = {
        // Posição                                Normal           Cor (cinza claro)
        -halfLength, sidewalkY, sidewalkZStart,  0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
         halfLength, sidewalkY, sidewalkZStart,  0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
         halfLength, sidewalkY, sidewalkZEnd,    0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
        
        -halfLength, sidewalkY, sidewalkZStart,  0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
         halfLength, sidewalkY, sidewalkZEnd,    0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
        -halfLength, sidewalkY, sidewalkZEnd,    0.0f, 1.0f, 0.0f,  0.6f, 0.6f, 0.6f,
    };
    
    sidewalkVertexCount = 6;
    
    glGenVertexArrays(1, &sidewalkVAO);
    glGenBuffers(1, &sidewalkVBO);
    
    glBindVertexArray(sidewalkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sidewalkVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Road::createLaneLinesMesh() {
    std::vector<float> vertices;
    
    float halfLength = roadLength / 2.0f;
    float dashLength = 2.0f;
    float gapLength = 1.5f;
    float lineWidth = 0.15f;
    float lineHeight = -0.26f;
    
    for (int i = 1; i < numLanes; i++) {
        float laneIndex1 = (i - 1) - numLanes/2;
        float laneIndex2 = i - numLanes/2;
        float z = (laneIndex1 + laneIndex2) * laneWidth / 2.0f;
        
        for (float x = -halfLength; x < halfLength; x += (dashLength + gapLength)) {
            float x1 = x;
            float x2 = std::min(x + dashLength, halfLength);
            
            std::vector<float> dash = {
                x1, lineHeight, z - lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
                x2, lineHeight, z - lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
                x2, lineHeight, z + lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
                
                x1, lineHeight, z - lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
                x2, lineHeight, z + lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
                x1, lineHeight, z + lineWidth/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
            };
            
            vertices.insert(vertices.end(), dash.begin(), dash.end());
        }
    }
    
    linesVertexCount = vertices.size() / 9;
    
    glGenVertexArrays(1, &linesVAO);
    glGenBuffers(1, &linesVBO);
    
    glBindVertexArray(linesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Road::render(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    shader.setVec3("lightPos", glm::vec3(10.0f, 20.0f, 10.0f));
    shader.setInt("useVertexColor", 1);
    
    // Renderizar chão da estrada
    glBindVertexArray(roadVAO);
    glDrawArrays(GL_TRIANGLES, 0, roadVertexCount);
    
    // Renderizar calçada
    glBindVertexArray(sidewalkVAO);
    glDrawArrays(GL_TRIANGLES, 0, sidewalkVertexCount);
    
    // Renderizar linhas divisórias
    glBindVertexArray(linesVAO);
    glDrawArrays(GL_TRIANGLES, 0, linesVertexCount);
    
    glBindVertexArray(0);
    shader.setInt("useVertexColor", 0);
}
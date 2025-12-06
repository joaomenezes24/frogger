#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

class Road {
public:
    Road(int numLanes = 5, float laneWidth = 2.5f, float roadLength = 120.0f);
    ~Road();
    
    void init();
    void render(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view);
    
    int getNumLanes() const { return numLanes; }
    float getLaneWidth() const { return laneWidth; }
    float getLaneZ(int lane) const; // retorna a posição Z de uma faixa específica
    float getSidewalkZ() const;     // Nova: retorna a posição Z da calçada
    
private:
    void createRoadMesh();
    void createLaneLinesMesh();
    void createSidewalkMesh();      // Nova: cria a calçada
    
    int numLanes;
    float laneWidth;
    float roadLength;
    float sidewalkWidth = 2.0f;     // Largura da calçada
    
    GLuint roadVAO, roadVBO;
    GLuint linesVAO, linesVBO;
    GLuint sidewalkVAO, sidewalkVBO; // Nova: buffers da calçada
    int roadVertexCount;
    int linesVertexCount;
    int sidewalkVertexCount;         // Nova: contador de vértices da calçada
};
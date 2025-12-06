#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Model.h"
#include "Road.h"

class Renderer {
public:
    Renderer() = default;
    bool init(int w, int h);
    void beginFrame();
    void endFrame();
    void renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale = {1,1,1});
    void renderRoad(Road &road);
    
    // Novo: função para ajustar a inclinação da câmera
    void setCameraTilt(float tilt);
    void setCameraDistance(float distance);
    
private:
    int width = 1280, height = 720;
    Shader shader;
    glm::mat4 projection;
    glm::mat4 view;
    
    // Novos: parâmetros da câmera
    float cameraTilt = 10.0f;      // Altura da câmera (quanto maior, mais de cima)
    float cameraDistance = 18.0f;  // Distância da câmera
    
    void updateCamera(); // Função auxiliar para atualizar a view matrix
};
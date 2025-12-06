#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

bool Renderer::init(int w, int h) {
    width = w; height = h;
    if (!shader.loadFromFiles("shaders/basic.vert", "shaders/basic.frag")) {
        std::cerr << "Failed to load shader\n";
        return false;
    }
    projection = glm::perspective(glm::radians(60.0f), width / (float)height, 0.1f, 200.0f);
    
    // Configura a câmera inicial
    updateCamera();
    
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Renderer::updateCamera() {
    // Posição da câmera baseada nos parâmetros
    glm::vec3 cameraPos = glm::vec3(0.0f, cameraTilt, cameraDistance);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

void Renderer::setCameraTilt(float tilt) {
    cameraTilt = tilt;
    updateCamera();
}

void Renderer::setCameraDistance(float distance) {
    cameraDistance = distance;
    updateCamera();
}

void Renderer::beginFrame() {
    glViewport(0,0,width,height);
    glClearColor(0.2f,0.6f,0.95f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // nothing here; swap handled in main
}

void Renderer::renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, scale);
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);
    shader.setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
    shader.setInt("useVertexColor", 0);
    m.draw();
}

void Renderer::renderRoad(Road &road) {
    road.render(shader, projection, view);
}
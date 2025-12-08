#pragma once

#include "Model.h"
#include <glm/glm.hpp>

class Renderer;

class Frog
{
public:
    Frog();
    void setModel(Model *m) { model = m; }
    void update(float dt);
    void render(Renderer &renderer);

    // Controles de movimento
    void moveForward();
    void moveBack();
    void moveLeft();
    void moveRight();

    // Getters e Setters de Posição
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3 &pos) { position = pos; }

    // Controle de escala
    void setScale(float s) { scale = glm::vec3(s, s, s); }

    // NOVO: Controle de rotação
    void setRotation(float yaw) { rotation = yaw; }
    float getRotation() const { return rotation; }

private:
    Model *model = nullptr;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    // ESCALA PADRÃO DO SAPO
    glm::vec3 scale{0.21f, 0.21f, 0.21f};

    // ROTAÇÃO DO SAPO (em graus)
    float rotation = 90.0f;
};
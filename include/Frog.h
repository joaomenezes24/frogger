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

    void moveForward();
    void moveBack();
    void moveLeft();
    void moveRight();

    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3 &pos) { position = pos; }

    void setScale(float s) { scale = glm::vec3(s, s, s); }

    void setRotation(float yaw) { rotation = yaw; }
    float getRotation() const { return rotation; }

private:
    Model *model = nullptr;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{0.21f, 0.21f, 0.21f};

    float rotation = 90.0f;
};
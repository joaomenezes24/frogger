#pragma once

#include "Model.h"
#include <glm/glm.hpp>

// Forward declaration to avoid including Renderer.h in this header.
class Renderer;

class Frog {
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
    void setPosition(const glm::vec3 &pos) { position = pos; }  // Nova função
private:
    Model *model = nullptr;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
};
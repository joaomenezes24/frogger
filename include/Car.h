#pragma once

#include "Model.h"
#include <glm/glm.hpp>

// Forward declaration: diz ao compilador que existe uma classe Renderer.
class Renderer;

class Car {
public:
    Car(Model *m = nullptr, float spd = 1.0f, int dir = 1);
    void setModel(Model *m) { model = m; }
    void update(float dt);
    void render(Renderer &renderer);
    glm::vec3 getPosition() const { return position; }
    bool isOffscreen() const;
    void setPosition(const glm::vec3 &p) { position = p; }
private:
    Model *model = nullptr;
    glm::vec3 position{0.0f,0.0f,0.0f};
    float speed = 1.0f;
    int direction = 1;
};

#include "Frog.h"
#include "Renderer.h"

Frog::Frog() {}

void Frog::update(float dt) {
    (void)dt;
    // nothing for now
}

void Frog::render(Renderer &renderer) {
    if (model) renderer.renderModel(*model, position, glm::vec3(1.0f));
}

void Frog::moveForward() { position.z -= 2.0f; }
void Frog::moveBack()    { position.z += 2.0f; }
void Frog::moveLeft()    { position.x -= 1.5f; }
void Frog::moveRight()   { position.x += 1.5f; }

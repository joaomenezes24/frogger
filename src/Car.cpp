#include "Car.h"
#include "Renderer.h"

Car::Car(Model *m, float spd, int dir) : model(m), speed(spd), direction(dir) {}

void Car::update(float dt) {
    position.x += speed * direction * dt;
}

void Car::render(Renderer &renderer) {
    if (model) renderer.renderModel(*model, position, glm::vec3(1.0f));
}

bool Car::isOffscreen() const {
    return position.x > 80.0f || position.x < -80.0f;
}

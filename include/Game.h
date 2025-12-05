#pragma once

#include <GLFW/glfw3.h>   // <<-- garante que GLFWwindow é visto de forma consistente
#include "Frog.h"
#include "Car.h"
#include "Renderer.h"
#include <vector>
#include <memory>

class Game {
public:
    Game() = default;
    ~Game() = default;
    bool init(int width, int height);

    // Use GLFWwindow* consistently (no 'class'/'struct' qualifier)
    void processInput(GLFWwindow *window);
    void update(float dt);
    void render(GLFWwindow *window);

private:
    Renderer renderer;
    Frog player;
    std::vector<std::unique_ptr<Car>> cars;
    Model carModelA, carModelB, frogModel;
    float spawnTimer = 0.0f;
};

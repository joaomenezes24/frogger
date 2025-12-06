#pragma once
#include "Renderer.h"
#include "Player.h"
#include "Car.h"
#include "Ground.h"
#include <vector>
#include <memory>

struct GLFWwindow;

class Game {
public:
    bool init(int width, int height);
    void processInput(GLFWwindow *window);
    void update(float dt);
    void render(GLFWwindow *window);

private:
    Renderer renderer;
    Ground ground;  // Adicionar o chão
    Player player;
    Model frogModel;
    Model carModelA, carModelB;
    std::vector<std::unique_ptr<Car>> cars;
    float spawnTimer = 0.0f;
};
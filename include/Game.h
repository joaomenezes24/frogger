#pragma once

// Forward declaration instead of including GLFW
struct GLFWwindow;

#include "Frog.h"
#include "Car.h"
#include "Renderer.h"
#include "Road.h"
#include <vector>
#include <memory>

class Game {
public:
    Game() = default;
    ~Game() = default;
    bool init(int width, int height);

    void processInput(GLFWwindow *window);
    void update(float dt);
    void render(GLFWwindow *window);

private:
    Renderer renderer;
    Frog player;
    std::vector<std::unique_ptr<Car>> cars;
    Model carModelA, carModelB, frogModel;
    Road road;
    float spawnTimer = 0.0f;
    
    // Configurações da estrada (MUDE ESTES VALORES)
    static constexpr int NUM_LANES = 5;        // Número de faixas
    static constexpr float LANE_WIDTH = 3.0f;  // Largura de cada faixa
    static constexpr float ROAD_LENGTH = 120.0f; // Comprimento total da estrada
    
    // Nova função: calcula velocidade baseada na faixa
    float getLaneSpeed(int lane) const;
};
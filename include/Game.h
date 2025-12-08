#pragma once

// Forward declaration
struct GLFWwindow;

#include "Frog.h"
#include "Car.h"
#include "Renderer.h"
#include "Road.h"
#include <vector>
#include <memory>
#include <string>

class Game
{
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

    // Vetor para guardar os 8 modelos de carro
    std::vector<Model> carModels;

    Model frogModel, wallModel, bushModel;

    Road road;
    float spawnTimer = 0.0f;
    float wallZ = 0.0f;
    
    // --- NOVO: Limite dos arbustos iniciais ---
    float startBushZ = 0.0f; 

    // Variáveis de Estado
    int lives = 5;
    bool gameWon = false;
    bool gameLost = false;
    GLFWwindow *gameWindow = nullptr;

    void resetGame();
    void updateTitle();
    bool canMoveTo(float newX, float newZ);

    static constexpr int NUM_LANES = 5;
    static constexpr float LANE_WIDTH = 3.0f;
    static constexpr float ROAD_LENGTH = 120.0f;

    float getLaneSpeed(int lane) const;
};
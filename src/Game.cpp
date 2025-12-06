#include "Game.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

bool Game::init(int width, int height) {
    if (!renderer.init(width, height)) return false;

    // Ajustar câmera - visão mais próxima e inclinada
    renderer.setCameraTilt(20.0f);      // Altura (quanto maior, mais de cima)
    renderer.setCameraDistance(13.0f);  // Distância (quanto menor, mais próximo)

    // Inicializar a estrada com os parâmetros configuráveis
    road = Road(NUM_LANES, LANE_WIDTH, ROAD_LENGTH);
    road.init();

    // Load models (assets should exist)
    if (!frogModel.load("assets/sapo.obj")) {
        std::cerr << "Warning: failed to load frog model\n";
    }
    
    // Carregar apenas o carModelB
    if (!carModelB.load("assets/LowPolyCars - 2.obj")) {
        std::cerr << "Warning: failed to load car model B\n";
    }

    player.setModel(&frogModel);
    
    // Posicionar o sapo na calçada inicial
    float sidewalkZ = road.getSidewalkZ();
    player.setPosition(glm::vec3(0.0f, 0.0f, sidewalkZ));

    // seed
    std::srand((unsigned)std::time(nullptr));

    // Carros iniciais com velocidades baseadas na faixa
    auto c1 = std::make_unique<Car>(&carModelB, getLaneSpeed(1), -1);
    c1->setPosition({40.0f, 0.1f, road.getLaneZ(1)});
    cars.push_back(std::move(c1));

    auto c2 = std::make_unique<Car>(&carModelB, getLaneSpeed(-1), -1);
    c2->setPosition({50.0f, 0.1f, road.getLaneZ(-1)});
    cars.push_back(std::move(c2));

    return true;
}

float Game::getLaneSpeed(int lane) const {
    // Define velocidade base por faixa
    // Faixas mais distantes da câmera (Z positivo) são MAIS LENTAS
    // Faixas mais próximas da câmera (Z negativo) são MAIS RÁPIDAS
    // Para NUM_LANES = 5: lanes são -2, -1, 0, 1, 2
    
    // Velocidade base mínima e máxima
    const float MIN_SPEED = 4.0f;   // Faixas distantes da câmera (Z positivo)
    const float MAX_SPEED = 12.0f;  // Faixas próximas à câmera (Z negativo)
    
    // Normaliza o índice da faixa para o intervalo [0, 1] INVERTIDO
    // lane = -2 → 1.0 (mais rápida, mais próxima)
    // lane = 0  → 0.5 (média)
    // lane = 2  → 0.0 (mais lenta, mais distante)
    int maxLane = NUM_LANES / 2;
    float normalizedLane = (maxLane - lane) / (float)(NUM_LANES - 1);
    
    // Interpolação linear INVERTIDA: Z negativo = MAX_SPEED, Z positivo = MIN_SPEED
    float speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * normalizedLane;
    
    // Adiciona pequena variação aleatória (±10%)
    float variation = speed * 0.1f;
    speed += (std::rand() % 200 - 100) / 1000.0f * variation;
    
    return speed;
}

void Game::processInput(GLFWwindow *window) {
    if (!window) return;
    static bool pressedW = false, pressedA=false, pressedS=false, pressedD=false;
    // We detect edge press to avoid continuous fast movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!pressedW) { player.moveForward(); pressedW = true; }
    } else pressedW = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!pressedS) { player.moveBack(); pressedS = true; }
    } else pressedS = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!pressedA) { player.moveLeft(); pressedA = true; }
    } else pressedA = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!pressedD) { player.moveRight(); pressedD = true; }
    } else pressedD = false;
}

void Game::update(float dt) {
    player.update(dt);
    for (auto &c : cars) c->update(dt);

    cars.erase(std::remove_if(cars.begin(), cars.end(), [](const std::unique_ptr<Car> &c){ return c->isOffscreen(); }), cars.end());

    spawnTimer += dt;
    if (spawnTimer > 1.2f) {
        spawnTimer = 0.0f;
        
        // Escolher uma faixa aleatória baseada no número de faixas
        int lane = (std::rand() % NUM_LANES) - NUM_LANES/2;
        float z = road.getLaneZ(lane);
        
        // Velocidade baseada na faixa
        float speed = getLaneSpeed(lane);
        
        // Direção sempre -1 (direita para esquerda)
        int dir = -1;
        
        // Usar apenas carModelB
        auto car = std::make_unique<Car>(&carModelB, speed, dir);
        
        // Carros sempre spawnam à direita (posição X positiva)
        // Y = 0.1 para ficar acima da estrada
        car->setPosition({60.0f, 0.1f, z});
        
        cars.push_back(std::move(car));
    }
}

void Game::render(GLFWwindow *window) {
    (void)window;
    renderer.beginFrame();

    // Renderizar a estrada primeiro
    renderer.renderRoad(road);

    // Renderizar jogador e carros
    player.render(renderer);
    for (auto &c : cars) c->render(renderer);

    renderer.endFrame();
}
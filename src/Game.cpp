#include "Game.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

bool Game::init(int width, int height) {
    if (!renderer.init(width, height)) return false;

    // Inicializar o chão com número de faixas configurável
    // Você pode mudar este número facilmente (3, 5, 7, 9, etc)
    int numLanes = 5;  // MUDE ESTE VALOR PARA ALTERAR O NÚMERO DE FAIXAS
    ground.init(numLanes);

    // Load models (assets should exist)
    if (!frogModel.load("assets/sapo.obj")) {
        std::cerr << "Warning: failed to load frog model\n";
    }
    if (!carModelA.load("assets/LowPolyCars.obj")) {
        std::cerr << "Warning: failed to load car model A\n";
    }
    if (!carModelB.load("assets/LowPolyCars - 2.obj")) {
        std::cerr << "Warning: failed to load car model B\n";
    }

    player.setModel(&frogModel);

    // seed
    std::srand((unsigned)std::time(nullptr));

    // initial car examples
    auto c1 = std::make_unique<Car>(&carModelA, 8.0f, -1);
    c1->setPosition({20.0f, 0.0f, 2.0f});
    cars.push_back(std::move(c1));

    auto c2 = std::make_unique<Car>(&carModelB, 6.0f, 1);
    c2->setPosition({-18.0f, 0.0f, -2.0f});
    cars.push_back(std::move(c2));

    return true;
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
        int numLanes = ground.getNumLanes();
        int lane = (std::rand() % numLanes) - (numLanes / 2); // Gera lanes baseado no número configurado
        float z = lane * 2.5f;
        float speed = 4.0f + (std::rand() % 6);
        int dir = (std::rand() % 2) ? 1 : -1;
        Model *m = (std::rand() % 2) ? &carModelA : &carModelB;
        auto car = std::make_unique<Car>(m, speed, dir);
        if (dir == 1) car->setPosition({-60.0f, 0.0f, z});
        else car->setPosition({60.0f, 0.0f, z});
        cars.push_back(std::move(car));
    }
}

void Game::render(GLFWwindow *window) {
    (void)window;
    renderer.beginFrame();

    // Renderizar o chão primeiro
    ground.render(renderer);

    player.render(renderer);
    for (auto &c : cars) c->render(renderer);

    renderer.endFrame();
    // swap buffers done in main
}
#include <iostream>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"

// No main.cpp, após game.init(), adicione esta linha:

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n"; return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280,720,"Froggy", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return -1;
    }

    Game game;
    if (!game.init(1280,720)) {
        std::cerr << "Game init failed\n"; return -1;
    }

    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();
    
    bool gameStarted = false;  // Flag para iniciar o jogo

    while (!glfwWindowShouldClose(window)) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        glfwPollEvents();
        game.processInput(window);
        
        // Iniciar o jogo no primeiro frame (após window estar pronta)
        if (!gameStarted) {
            game.startGame();
            gameStarted = true;
        }
        
        game.update(dt);
        game.render(window);

        glfwSwapBuffers(window);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    glfwTerminate();
    return 0;
}
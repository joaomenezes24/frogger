#include "Game.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int loadTextureFromFile(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "ERRO: Textura nao encontrada: " << path << "\n";
        stbi_image_free(data);
    }

    return textureID;
}

bool Game::init(int width, int height)
{
    if (!renderer.init(width, height))
        return false;

    renderer.setCameraTilt(20.0f);
    renderer.setCameraDistance(10.0f);

    road = Road(NUM_LANES, LANE_WIDTH, ROAD_LENGTH);
    road.init();

    float lastLaneZ = (NUM_LANES / 2) * LANE_WIDTH;
    wallZ = -(lastLaneZ + 1.9f);

    float sidewalkZ = road.getSidewalkZ();
    startBushZ = sidewalkZ + 1.5f;

    if (!frogModel.load("assets/sapo.obj"))
        std::cerr << "Erro sapo.obj\n";
    if (!wallModel.load("assets/muro.obj"))
        std::cerr << "Erro muro.obj\n";
    if (!bushModel.load("assets/arbustos.obj"))
        std::cerr << "Erro arbustos.obj\n";

    // Textura do Sapo
    unsigned int frogTexID = loadTextureFromFile("assets/frog_diff.jpg");
    frogModel.setTextureID(frogTexID);

    //TEXTURA DO MURO
    unsigned int wallTexID = loadTextureFromFile("assets/wall_aged.png");
    wallModel.setTextureID(wallTexID);

    int numCarModels = 8;
    carModels.resize(numCarModels);

    for (int i = 0; i < numCarModels; i++)
    {
        if (!carModels[i].load("assets/LowPolyCars - 2.obj"))
        {
            std::cerr << "Erro ao carregar modelo do carro " << i << "\n";
        }
        std::string texName = "assets/Car Texture " + std::to_string(i + 1) + ".png";
        unsigned int texID = loadTextureFromFile(texName.c_str());
        carModels[i].setTextureID(texID);
    }

    player.setModel(&frogModel);

    std::srand((unsigned)std::time(nullptr));

    // Inicializar linhas de limite
    float sidewalkZForBoundary = road.getSidewalkZ();
    float boundaryStartZ = sidewalkZForBoundary;
    float boundaryEndZ = wallZ - 3.0f;
    
    leftBoundary.init(LEFT_BOUNDARY_X, boundaryStartZ, boundaryEndZ);
    rightBoundary.init(RIGHT_BOUNDARY_X, boundaryStartZ, boundaryEndZ);
    
    // Inicializar valores padrão
    lives = 5;
    gameWon = false;
    gameLost = false;
    countdownActive = false;

    return true;
}

void Game::startGame()
{
    lives = 5;
    gameWon = false;
    gameLost = false;
    spawnTimer = 0.0f;
    
    countdownActive = true;
    countdownTimer = 5.0f;
    countdownNumber = 5;

    cars.clear();

    float sidewalkZ = road.getSidewalkZ();
    player.setPosition(glm::vec3(0.0f, 0.33f, sidewalkZ));

    int r1 = std::rand() % 8;
    auto c1 = std::make_unique<Car>(&carModels[r1], getLaneSpeed(1), -1);
    c1->setPosition({40.0f, 0.1f, road.getLaneZ(1)});
    cars.push_back(std::move(c1));

    int r2 = std::rand() % 8;
    auto c2 = std::make_unique<Car>(&carModels[r2], getLaneSpeed(-1), -1);
    c2->setPosition({50.0f, 0.1f, road.getLaneZ(-1)});
    cars.push_back(std::move(c2));
}

// SUBSTITUA a função resetGame no Game.cpp por esta:

void Game::resetGame()
{
    startGame();
    updateTitle();  // Atualizar título imediatamente
}


// SUBSTITUA a função updateTitle no Game.cpp por esta:

void Game::updateTitle()
{
    if (!gameWindow)
        return;

    if (gameWon)
        glfwSetWindowTitle(gameWindow, "VOCE GANHOU! APERTE ENTER PARA TENTAR NOVAMENTE");
    else if (gameLost)
        glfwSetWindowTitle(gameWindow, "VOCE PERDEU! PARA JOGAR NOVAMENTE APERTE ENTER");
    else if (countdownActive)
    {
        std::string title = "O JOGO VAI COMECAR EM: " + std::to_string(countdownNumber);
        glfwSetWindowTitle(gameWindow, title.c_str());
    }
    else
    {
        std::string title = "VIDAS - " + std::to_string(lives) + "/5";
        glfwSetWindowTitle(gameWindow, title.c_str());
    }
}

bool Game::canMoveTo(float newX, float newZ)
{
    // Verificar limites laterais (posição das linhas amarelas)
    if (newX < LEFT_BOUNDARY_X || newX > RIGHT_BOUNDARY_X)
        return false;
    
    // Verificar limite superior (arbustos)
    if (newZ > startBushZ)
        return false;

    // Verificar colisão com o muro
    float wallThickness = 1.5f;
    bool isInsideWallZone = (newZ <= wallZ + wallThickness) && (newZ >= wallZ - wallThickness);

    if (isInsideWallZone)
    {
        float gapCenter = 6.0f;
        float gapWidth = 4.0f;
        float gapMin = gapCenter - (gapWidth / 2.0f);
        float gapMax = gapCenter + (gapWidth / 2.0f);

        if (newX < gapMin || newX > gapMax)
            return false;
    }
    
    return true;
}
float Game::getLaneSpeed(int lane) const
{
    const float MIN_SPEED = 4.0f;
    const float MAX_SPEED = 12.0f;
    int maxLane = NUM_LANES / 2;
    float normalizedLane = (maxLane - lane) / (float)(NUM_LANES - 1);
    float speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * normalizedLane;
    float variation = speed * 0.1f;
    speed += (std::rand() % 200 - 100) / 1000.0f * variation;
    return speed;
}

void Game::processInput(GLFWwindow *window)
{
    if (!window)
        return;
    gameWindow = window;

    if (gameWon || gameLost)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            resetGame();
        return;
    }
    
    // BLOQUEAR MOVIMENTO DURANTE COUNTDOWN
    if (countdownActive)
        return;

    // ... resto do código de processInput continua normal ...
    static bool pressedForward = false, pressedLeft = false, pressedBack = false, pressedRight = false;
    glm::vec3 currentPos = player.getPosition();
    float step = 3.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (!pressedForward)
        {
            if (canMoveTo(currentPos.x, currentPos.z - step))
            {
                player.moveForward();
                if (player.getPosition().z < wallZ - 1.0f)
                {
                    gameWon = true;
                    updateTitle();
                }
            }
            pressedForward = true;
        }
    }
    else
        pressedForward = false;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (!pressedBack)
        {
            if (canMoveTo(currentPos.x, currentPos.z + step))
                player.moveBack();
            pressedBack = true;
        }
    }
    else
        pressedBack = false;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (!pressedLeft)
        {
            if (canMoveTo(currentPos.x - step, currentPos.z))
                player.moveLeft();
            pressedLeft = true;
        }
    }
    else
        pressedLeft = false;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (!pressedRight)
        {
            if (canMoveTo(currentPos.x + step, currentPos.z))
                player.moveRight();
            pressedRight = true;
        }
    }
    else
        pressedRight = false;
    
    if (countdownActive)
    return;
}

// SUBSTITUA a função update no Game.cpp por esta:

// SUBSTITUA a função update no Game.cpp por esta:

// SUBSTITUA a função update no Game.cpp por esta:

void Game::update(float dt)
{
    if (gameWon || gameLost)
        return;

    // PROCESSAR COUNTDOWN
    if (countdownActive)
    {
        countdownTimer -= dt;
        
        // Atualizar o número exibido baseado no tempo restante
        int oldNumber = countdownNumber;
        
        if (countdownTimer > 4.0f)
            countdownNumber = 5;
        else if (countdownTimer > 3.0f)
            countdownNumber = 4;
        else if (countdownTimer > 2.0f)
            countdownNumber = 3;
        else if (countdownTimer > 1.0f)
            countdownNumber = 2;
        else if (countdownTimer > 0.0f)
            countdownNumber = 1;
        
        // Atualizar título quando o número muda
        if (oldNumber != countdownNumber)
        {
            updateTitle();
            std::cout << "Countdown: " << countdownNumber << std::endl;
        }
        
        // Terminar countdown
        if (countdownTimer <= 0.0f)
        {
            countdownActive = false;
            countdownTimer = 0.0f;
            updateTitle(); // Atualizar para mostrar vidas
            std::cout << "Countdown terminado! Jogo iniciado!" << std::endl;
        }
        
        // Carros continuam se movendo durante countdown
        for (auto &c : cars)
            c->update(dt);
        
        // Remover carros fora da tela
        cars.erase(std::remove_if(cars.begin(), cars.end(), [](const std::unique_ptr<Car> &c)
                                  { return c->isOffscreen(); }),
                   cars.end());
        
        // Spawn de carros continua
        spawnTimer += dt;
        if (spawnTimer > 0.4f)
        {
            int lane = (std::rand() % NUM_LANES) - NUM_LANES / 2;
            float z = road.getLaneZ(lane);
            float spawnX = 60.0f;

            bool isSafe = true;
            float safeDist = 15.0f;

            for (const auto &otherCar : cars)
            {
                glm::vec3 pos = otherCar->getPosition();
                if (std::abs(pos.z - z) < 0.5f)
                {
                    if (std::abs(pos.x - spawnX) < safeDist)
                    {
                        isSafe = false;
                        break;
                    }
                }
            }

            if (isSafe)
            {
                spawnTimer = 0.0f;
                float speed = getLaneSpeed(lane);
                int dir = -1;
                int randomModelIndex = std::rand() % 8;
                auto car = std::make_unique<Car>(&carModels[randomModelIndex], speed, dir);
                car->setPosition({spawnX, 0.1f, z});
                cars.push_back(std::move(car));
            }
            else
                spawnTimer = 0.5f;
        }
        
        return; // Não processar colisão durante countdown
    }

    // JOGO NORMAL (após countdown)
    player.update(dt);

    glm::vec3 playerPos = player.getPosition();
    float hitboxX = 2.2f;
    float hitboxZ = 0.8f;

    for (auto &c : cars)
    {
        c->update(dt);
        glm::vec3 carPos = c->getPosition();

        if (std::abs(playerPos.z - carPos.z) < hitboxZ)
        {
            if (std::abs(playerPos.x - carPos.x) < hitboxX)
            {
                lives--;
                if (lives > 0)
                {
                    float startZ = road.getSidewalkZ();
                    player.setPosition(glm::vec3(0.0f, 0.33f, startZ));
                    std::cout << "Vida Perdida! Restam: " << lives << "\n";
                    updateTitle();
                }
                else
                {
                    gameLost = true;
                    updateTitle();
                }
            }
        }
    }

    cars.erase(std::remove_if(cars.begin(), cars.end(), [](const std::unique_ptr<Car> &c)
                              { return c->isOffscreen(); }),
               cars.end());

    spawnTimer += dt;
    if (spawnTimer > 0.4f)
    {
        int lane = (std::rand() % NUM_LANES) - NUM_LANES / 2;
        float z = road.getLaneZ(lane);
        float spawnX = 60.0f;

        bool isSafe = true;
        float safeDist = 15.0f;

        for (const auto &otherCar : cars)
        {
            glm::vec3 pos = otherCar->getPosition();
            if (std::abs(pos.z - z) < 0.5f)
            {
                if (std::abs(pos.x - spawnX) < safeDist)
                {
                    isSafe = false;
                    break;
                }
            }
        }

        if (isSafe)
        {
            spawnTimer = 0.0f;
            float speed = getLaneSpeed(lane);
            int dir = -1;
            int randomModelIndex = std::rand() % 8;
            auto car = std::make_unique<Car>(&carModels[randomModelIndex], speed, dir);
            car->setPosition({spawnX, 0.1f, z});
            cars.push_back(std::move(car));
        }
        else
            spawnTimer = 0.5f;
    }
}

void Game::render(GLFWwindow *window)
{
    (void)window;
    renderer.beginFrame();

    // 1. RENDERIZAR ESTRADA PRIMEIRO (com suas cores)
    renderer.renderRoad(road);

    // 2. RENDERIZAR MURO
    glm::mat4 wallModelMatrix = glm::mat4(1.0f);
    wallModelMatrix = glm::translate(wallModelMatrix, glm::vec3(-7.5f, -0.4f, wallZ));
    wallModelMatrix = glm::rotate(wallModelMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    wallModelMatrix = glm::rotate(wallModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    wallModelMatrix = glm::scale(wallModelMatrix, glm::vec3(2.7f, 2.7f, 2.7f));
    renderer.renderModelWithMatrix(wallModel, wallModelMatrix, glm::vec3(0.3f, 0.15f, 0.05f));

    // 3. RENDERIZAR FLORESTA
    float startForestZ = wallZ - 0.99f;
    float rowSpacing = 2.0f;
    for (int i = 0; i < 5; i++)
    {
        float currentZ = startForestZ - (i * rowSpacing);
        glm::mat4 forestMatrix = glm::mat4(1.0f);
        forestMatrix = glm::translate(forestMatrix, glm::vec3(0.0f, 0.0f, currentZ));
        forestMatrix = glm::scale(forestMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
        renderer.renderModelWithMatrix(bushModel, forestMatrix, glm::vec3(0.0f, 0.4f, 0.0f));
    }

    // 4. RENDERIZAR ARBUSTOS DO INÍCIO
    float sidewalkZ = road.getSidewalkZ();
    float bushZ = sidewalkZ + 2.45f;
    glm::mat4 bushModelMatrix = glm::mat4(1.0f);
    bushModelMatrix = glm::translate(bushModelMatrix, glm::vec3(3.0f, 0.0f, bushZ));
    bushModelMatrix = glm::scale(bushModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    renderer.renderModelWithMatrix(bushModel, bushModelMatrix, glm::vec3(0.1f, 0.6f, 0.1f));

    // 5. RENDERIZAR SAPO E CARROS
    player.render(renderer);
    for (auto &c : cars)
        c->render(renderer);

    // 6. RENDERIZAR LINHAS DE LIMITE POR ÚLTIMO (para garantir que fiquem visíveis)
    renderer.renderBoundaryLines(leftBoundary);
    renderer.renderBoundaryLines(rightBoundary);

    renderer.endFrame();
}
#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

bool Renderer::init(int w, int h)
{
    width = w;
    height = h;
    if (!shader.loadFromFiles("shaders/basic.vert", "shaders/basic.frag"))
        return false;

    projection = glm::perspective(glm::radians(60.0f), width / (float)height, 0.1f, 200.0f);
    updateCamera();

    // Habilita teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Habilita Transparência (BLEND) para a sombra
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Renderer::updateCamera()
{
    glm::vec3 cPos = glm::vec3(0.0f, cameraTilt, cameraDistance);
    view = glm::lookAt(cPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::setCameraTilt(float t)
{
    cameraTilt = t;
    updateCamera();
}
void Renderer::setCameraDistance(float d)
{
    cameraDistance = d;
    updateCamera();
}

void Renderer::beginFrame()
{
    glViewport(0, 0, width, height);
    glClearColor(0.55f, 0.27f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {}

// --- CONFIGURAÇÃO DE MATERIAL ---
void setupMaterial(const Shader &shader, const Model *model, glm::vec3 color, bool forceVertexColor = false, bool isShadow = false)
{
    shader.setVec3("lightPos", glm::vec3(10.0f, 20.0f, 10.0f));

    if (isShadow)
    {
        // MODO SOMBRA
        shader.setBool("useTexture", false);
        shader.setBool("useVertexColor", false);
        shader.setBool("useLighting", false);                       // Sem luz na sombra
        shader.setFloat("alpha", 0.6f);                             // Transparente
        shader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f)); // Preto
    }
    else
    {
        // MODO NORMAL
        shader.setBool("useLighting", true);
        shader.setFloat("alpha", 1.0f); // Opaco

        if (forceVertexColor)
        {
            // ESTRADA
            shader.setBool("useTexture", false);
            shader.setBool("useVertexColor", true);
        }
        else if (model && model->getTextureID() != 0)
        {
            // CARRO / SAPO
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->getTextureID());
            shader.setInt("texture1", 0);
            shader.setBool("useTexture", true);
            shader.setBool("useVertexColor", false);
        }
        else
        {
            // MURO / ARBUSTO
            shader.setBool("useTexture", false);
            shader.setBool("useVertexColor", false);
            shader.setVec3("objectColor", color);
        }
    }
}

// Renderiza Estrada
void Renderer::renderRoad(Road &road)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // Ativa Vertex Color para ver as listras
    setupMaterial(shader, nullptr, glm::vec3(0), true);

    road.render(shader, projection, view);
}

// Renderiza Carros (Com Sombra)
void Renderer::renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale, glm::vec3 color)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // 1. Objeto Normal
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, scale);
    shader.setMat4("model", model);
    setupMaterial(shader, &m, color);
    m.draw();

    // 2. Sombra
    glm::mat4 shadowModel = glm::mat4(1.0f);
    shadowModel = glm::translate(shadowModel, glm::vec3(pos.x, 0.03f, pos.z));
    shadowModel = glm::scale(shadowModel, glm::vec3(scale.x, 0.01f, scale.z));

    shader.setMat4("model", shadowModel);
    setupMaterial(shader, nullptr, glm::vec3(0), false, true); // isShadow = true
    m.draw();

    glBindTexture(GL_TEXTURE_2D, 0);
}

// Renderiza Sapo (Com Sombra)
void Renderer::renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale, float rotationY, glm::vec3 color)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // 1. Objeto
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    setupMaterial(shader, &m, color);
    m.draw();

    // 2. Sombra
    glm::mat4 shadowModel = glm::mat4(1.0f);
    shadowModel = glm::translate(shadowModel, glm::vec3(pos.x, 0.03f, pos.z));
    shadowModel = glm::rotate(shadowModel, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    // Sombra um pouco menor no sapo para ficar bonito
    shadowModel = glm::scale(shadowModel, glm::vec3(scale.x * 0.8f, 0.01f, scale.z * 0.8f));

    shader.setMat4("model", shadowModel);
    setupMaterial(shader, nullptr, glm::vec3(0), false, true);
    m.draw();

    glBindTexture(GL_TEXTURE_2D, 0);
}

// Muro/Arbusto (Sem sombra)
void Renderer::renderModelWithMatrix(const Model &model, glm::mat4 matrix, glm::vec3 color)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", matrix);

    setupMaterial(shader, &model, color);
    model.draw();
    glBindTexture(GL_TEXTURE_2D, 0);
}
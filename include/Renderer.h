#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Model.h"
#include "Road.h"
#include "BoundaryLines.h"


class Renderer
{
public:
    Renderer() = default;
    bool init(int w, int h);
    void beginFrame();
    void endFrame();

    void setCameraTilt(float tilt);
    void setCameraDistance(float distance);
    void renderBoundaryLines(BoundaryLines& lines);


    // Para Carros (Padrão: Branco/Sem tintura extra)
    void renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale, glm::vec3 color = glm::vec3(1.0f));

    // Para Sapo (Padrão: Branco/Sem tintura extra)
    void renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale, float rotationY, glm::vec3 color = glm::vec3(1.0f));

    // Para Muro e Arbustos (Aqui passaremos a cor desejada)
    void renderModelWithMatrix(const Model &model, glm::mat4 matrix, glm::vec3 color);

    void renderRoad(Road &road);

private:
    void updateCamera();

    int width, height;
    float cameraTilt = 0.0f;
    float cameraDistance = 0.0f;

    Shader shader;
    glm::mat4 projection;
    glm::mat4 view;
};
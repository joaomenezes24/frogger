#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Model.h"

class Renderer {
public:
    Renderer() = default;
    bool init(int w, int h);
    void beginFrame();
    void endFrame();
    void renderModel(const Model &m, const glm::vec3 &pos, const glm::vec3 &scale = {1,1,1});
private:
    int width = 1280, height = 720;
    Shader shader;
    glm::mat4 projection;
};

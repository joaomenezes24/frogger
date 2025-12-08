#include "Frog.h"
#include "Renderer.h"
#include <glm/glm.hpp>

Frog::Frog() {
    // DIREÇÃO PADRÃO DO SAPO (OLHANDO PRA CIMA)
    rotation = 90.0f; 
}

void Frog::update(float dt) {
    (void)dt; // SE O SAPO NÃO PULA, NÃO ATUALIZA
}

void Frog::render(Renderer &renderer) {
    if (model) {
        // (LEMBRAR DE CORRIGIR A DESGRAÇA DO MODELO DEPOIS. Ele fica verde caso o modelo falhe)
        renderer.renderModel(*model, position, scale, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

// MOVIMENTAÇÃO

void Frog::moveForward() { 
    position.z -= 3.0f; 
}

void Frog::moveBack() { 
    position.z += 3.0f; 
}

void Frog::moveLeft() { 
    position.x -= 3.0f; 
}

void Frog::moveRight() { 
    position.x += 3.0f; 
}
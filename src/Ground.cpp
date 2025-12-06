#include "Ground.h"
#include "Renderer.h"
#include <vector>
#include <iostream>

Ground::Ground() : VAO(0), VBO(0), EBO(0), indexCount(0), numLanes(5) {}
std::cout << "===== GROUND CONSTRUTOR CHAMADO =====\n";

Ground::~Ground() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Ground::init(int lanes) {
    numLanes = lanes;
    
    std::cout << "Ground::init() - Inicializando com " << lanes << " faixas\n";
    
    // Carregar shader específico para o chão
    if (!groundShader.loadFromFiles("shaders/ground.vert", "shaders/ground.frag")) {
        std::cerr << "ERRO: Failed to load ground shader\n";
        return;
    }
    
    std::cout << "Ground shader carregado com sucesso\n";
    
    createGroundMesh();
    
    std::cout << "Ground mesh criado: VAO=" << VAO << ", indexCount=" << indexCount << "\n";
}

void Ground::createGroundMesh() {
    // Estrutura de vértice: posição (3) + normal (3) + uv (2) + cor (3)
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color;
    };
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Cores
    glm::vec3 roadColor(0.2f, 0.2f, 0.25f);      // Cinza escuro para asfalto
    glm::vec3 lineColor(0.9f, 0.9f, 0.3f);       // Amarelo para faixas
    glm::vec3 grassColor(0.2f, 0.6f, 0.2f);      // Verde para grama
    
    // Dimensões
    float roadWidth = 100.0f;  // Largura da estrada
    float roadLength = 2.2f;   // Largura de cada faixa (lane)
    float lineWidth = 0.15f;   // Largura das linhas amarelas
    float spacing = 0.3f;      // Espaçamento entre faixas
    
    glm::vec3 normal(0.0f, 1.0f, 0.0f); // Normal apontando para cima
    
    auto addQuad = [&](float x1, float z1, float x2, float z2, const glm::vec3& color) {
        unsigned int baseIdx = vertices.size();
        
        vertices.push_back({{x1, 0.0f, z1}, normal, {0.0f, 0.0f}, color});
        vertices.push_back({{x2, 0.0f, z1}, normal, {1.0f, 0.0f}, color});
        vertices.push_back({{x2, 0.0f, z2}, normal, {1.0f, 1.0f}, color});
        vertices.push_back({{x1, 0.0f, z2}, normal, {0.0f, 1.0f}, color});
        
        indices.push_back(baseIdx + 0);
        indices.push_back(baseIdx + 1);
        indices.push_back(baseIdx + 2);
        indices.push_back(baseIdx + 0);
        indices.push_back(baseIdx + 2);
        indices.push_back(baseIdx + 3);
    };
    
    // Renderizar faixas da estrada com espaçamento
    for (int i = 0; i < numLanes; i++) {
        // Calcular o índice da faixa centralizado (ex: 5 faixas = -2,-1,0,1,2)
        int laneIndex = i - (numLanes / 2);
        float centerZ = laneIndex * 2.5f; // Cada lane está a 2.5 unidades de distância
        
        float z1 = centerZ - roadLength / 2.0f;
        float z2 = centerZ + roadLength / 2.0f;
        
        // Faixa de asfalto
        addQuad(-roadWidth, z1, roadWidth, z2, roadColor);
        
        // Linha amarela no topo da faixa (se não for a última)
        if (i < numLanes - 1) {
            float lineZ = z2 + spacing / 2.0f;
            addQuad(-roadWidth, lineZ - lineWidth/2.0f, roadWidth, lineZ + lineWidth/2.0f, lineColor);
        }
    }
    
    // Grama nas laterais (acima e abaixo das faixas)
    int minLaneIndex = -(numLanes / 2);
    int maxLaneIndex = (numLanes - 1) - (numLanes / 2);
    
    float firstLaneStart = minLaneIndex * 2.5f - roadLength / 2.0f;
    float lastLaneEnd = maxLaneIndex * 2.5f + roadLength / 2.0f;
    
    // Grama superior
    addQuad(-roadWidth, lastLaneEnd + spacing, roadWidth, lastLaneEnd + 20.0f, grassColor);
    
    // Grama inferior
    addQuad(-roadWidth, firstLaneStart - 20.0f, roadWidth, firstLaneStart - spacing, grassColor);
    
    // Criar buffers OpenGL
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Layout: 0=pos, 1=normal, 2=uv, 3=color
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    
    glBindVertexArray(0);
    
    indexCount = indices.size();
}

void Ground::render(Renderer& renderer) {
    if (VAO == 0 || indexCount == 0) {
        std::cerr << "Ground::render() - VAO ou indexCount inválidos!\n";
        return;
    }
    
    groundShader.use();
    
    // Configurar matrizes
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, 18.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0,1,0));
    
    groundShader.setMat4("projection", renderer.getProjection());
    groundShader.setMat4("view", view);
    groundShader.setMat4("model", model);
    groundShader.setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
    
    // Renderizar
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
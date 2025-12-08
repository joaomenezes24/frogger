#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Se você usar a classe Shader no draw, precisaria incluir ou declarar aqui.
// Por enquanto, mantive como no seu snippet original.
class Shader;

struct Mesh
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int indexCount = 0;
};

class Model
{
public:
    Model() = default;
    explicit Model(const std::string &path);
    ~Model();

    bool load(const std::string &path);

    // Se o seu Renderer passa o shader, mude para: void draw(Shader &shader) const;
    void draw() const;

    // --- NOVOS MÉTODOS PARA TEXTURA ---

    // Define o ID da textura (gerado pelo loadTextureFromFile)
    void setTextureID(unsigned int id) { textureID = id; }

    // Retorna o ID para o Renderer usar noglBindTexture
    unsigned int getTextureID() const { return textureID; }

private:
    std::vector<Mesh> meshes;

    // Guarda o ID da textura OpenGL. 0 significa "Sem Textura".
    unsigned int textureID = 0;
};
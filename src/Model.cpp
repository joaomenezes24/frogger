#include "Model.h"
#include <glad/glad.h>
#include <iostream>

// tinyobjloader header: you can either install via vcpkg (tinyobjloader) or
// drop tiny_obj_loader.h into include/ (rename if needed).
#include "tiny_obj_loader.h"

Model::Model(const std::string &path) { load(path); }

Model::~Model() {
    for (auto &m : meshes) {
        if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
        if (m.VBO) glDeleteBuffers(1, &m.VBO);
        if (m.EBO) glDeleteBuffers(1, &m.EBO);
    }
}

bool Model::load(const std::string &path) {
    meshes.clear();
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string basedir; // tinyobj will read .mtl using the .obj path automatically
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), nullptr, true);
    if (!warn.empty()) std::cout << "tinyobj warn: " << warn << "\n";
    if (!err.empty()) std::cerr << "tinyobj err: " << err << "\n";
    if (!ret) {
        std::cerr << "Failed to load/parse .obj: " << path << "\n";
        return false;
    }

    // For each shape create a mesh
    for (const auto &shape : shapes) {
        struct Vertex { glm::vec3 pos; glm::vec3 normal; glm::vec2 uv; };
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        std::unordered_map<uint64_t, unsigned int> uniqueVertices;
        auto pack_index = [](int a, int b, int c) -> uint64_t {
            return ( (uint64_t)(uint32_t)a ) | ( (uint64_t)(uint32_t)b << 21 ) | ( (uint64_t)(uint32_t)c << 42 );
        };

        for (const auto &idx : shape.mesh.indices) {
            Vertex v{};
            int vi = idx.vertex_index;
            if (vi >= 0) {
                v.pos = { attrib.vertices[3*vi+0], attrib.vertices[3*vi+1], attrib.vertices[3*vi+2] };
            }
            int ni = idx.normal_index;
            if (ni >= 0) {
                v.normal = { attrib.normals[3*ni+0], attrib.normals[3*ni+1], attrib.normals[3*ni+2] };
            } else {
                v.normal = {0.0f,1.0f,0.0f};
            }
            int ti = idx.texcoord_index;
            if (ti >= 0) {
                v.uv = { attrib.texcoords[2*ti+0], attrib.texcoords[2*ti+1] };
            } else v.uv = {0.0f,0.0f};

            // Pack indices to unique key to avoid duplicates
            uint64_t key = pack_index(vi, ni, ti);
            auto it = uniqueVertices.find(key);
            if (it != uniqueVertices.end()) {
                indices.push_back(it->second);
            } else {
                unsigned int newIndex = (unsigned int)vertices.size();
                vertices.push_back(v);
                uniqueVertices[key] = newIndex;
                indices.push_back(newIndex);
            }
        }

        Mesh mesh;
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // layout: 0 pos, 1 normal, 2 uv
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

        glBindVertexArray(0);

        mesh.indexCount = (unsigned int)indices.size();
        meshes.push_back(mesh);
    }

    return true;
}

void Model::draw() const {
    for (const auto &m : meshes) {
        glBindVertexArray(m.VAO);
        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

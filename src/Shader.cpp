#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

static std::string readFile(const std::string &path) {
    std::ifstream in(path);
    if (!in) return std::string();
    std::stringstream ss; ss << in.rdbuf();
    return ss.str();
}

Shader::~Shader() {
    if (ID) glDeleteProgram(ID);
}

bool Shader::loadFromFiles(const std::string &vertPath, const std::string &fragPath) {
    std::string vsrc = readFile(vertPath);
    std::string fsrc = readFile(fragPath);
    if (vsrc.empty() || fsrc.empty()) {
        std::cerr << "Shader files not found: " << vertPath << " " << fragPath << "\n";
        return false;
    }

    const char *v = vsrc.c_str();
    const char *f = fsrc.c_str();

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &v, nullptr);
    glCompileShader(vs);
    int success; glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buf[1024]; glGetShaderInfoLog(vs, 1024, nullptr, buf);
        std::cerr << "Vertex shader compile error:\n" << buf << "\n";
        return false;
    }

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &f, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buf[1024]; glGetShaderInfoLog(fs, 1024, nullptr, buf);
        std::cerr << "Fragment shader compile error:\n" << buf << "\n";
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char buf[1024]; glGetProgramInfoLog(ID, 1024, nullptr, buf);
        std::cerr << "Shader link error:\n" << buf << "\n";
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

void Shader::use() const { if (ID) glUseProgram(ID); }

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    int loc = glGetUniformLocation(ID, name.c_str()); 
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &v) const {
    int loc = glGetUniformLocation(ID, name.c_str()); 
    if (loc != -1) glUniform3f(loc, v.x, v.y, v.z);
}

void Shader::setInt(const std::string &name, int value) const {
    int loc = glGetUniformLocation(ID, name.c_str()); 
    if (loc != -1) glUniform1i(loc, value);
}
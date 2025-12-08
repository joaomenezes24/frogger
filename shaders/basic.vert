#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aAttr2; // <--- TRUQUE: Aceita tanto Cor (RGB) quanto UV (XY)

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Repassa os dados para o fragment shader
    TexCoords = aAttr2.xy; // Se for textura, usa XY
    VertexColor = aAttr2;  // Se for cor, usa RGB
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
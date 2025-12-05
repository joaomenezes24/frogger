#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;

void main() {
    vec3 color = vec3(0.7, 0.7, 0.7); // neutral color; OBJ may not supply textures here
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 result = color * diff;
    FragColor = vec4(result, 1.0);
}

#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

out vec4 FragColor;

uniform vec3 lightPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Iluminação ambiente
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * VertexColor;
    
    // Iluminação difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * VertexColor;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

uniform vec3 lightPos;
uniform int useVertexColor; // 0 = usar cor padrão, 1 = usar VertexColor

void main() {
    // Iluminação básica
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    
    vec3 ambient = 0.3 * vec3(1.0);
    vec3 diffuse = diff * vec3(1.0);
    
    vec3 baseColor;
    if (useVertexColor == 1) {
        baseColor = VertexColor;
    } else {
        baseColor = vec3(0.5, 0.5, 0.5); // Cor padrão cinza
    }
    
    vec3 result = (ambient + diffuse) * baseColor;
    FragColor = vec4(result, 1.0);
}
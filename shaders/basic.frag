#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 VertexColor;

uniform vec3 lightPos;
uniform sampler2D texture1;
uniform bool useTexture;
uniform bool useVertexColor;
uniform vec3 objectColor;

// --- NOVOS UNIFORMS ---
uniform float alpha;       // Transparência (0.0 a 1.0)
uniform bool useLighting;  // Liga/Desliga luz (Falso para sombras)

void main() {
    vec3 resultColor;

    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        if(texColor.a < 0.1) discard;
        resultColor = texColor.rgb;
    } 
    else if (useVertexColor) {
        resultColor = VertexColor;
    }
    else {
        resultColor = objectColor;
    }
    
    // --- LÓGICA DE ILUMINAÇÃO ---
    if (useLighting) {
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(normalize(Normal), lightDir), 0.0);
        vec3 ambient = 0.5 * vec3(1.0);
        vec3 diffuse = diff * vec3(1.0);
        resultColor = (ambient + diffuse) * resultColor;
    }
    // Se useLighting for false (sombra), usamos a cor pura (preto) sem luz atrapalhando.

    FragColor = vec4(resultColor, alpha); // Usa o alpha que mandamos
}
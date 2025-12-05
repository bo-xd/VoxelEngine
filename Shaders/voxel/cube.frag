#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir);
    float diff = max(dot(norm, light), 0.0);
    vec3 ambient = Color * 0.3;
    vec3 diffuse = Color * diff * 0.7;
    FragColor = vec4(ambient + diffuse, 1.0);
}

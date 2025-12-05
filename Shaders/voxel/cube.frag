#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 blockColor;

struct DirLight {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};
uniform DirLight dirLight;
uniform vec3 viewPos;

void main() {
    vec3 ambient = dirLight.ambient * blockColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * dirLight.diffuse * blockColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = dirLight.specular * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}

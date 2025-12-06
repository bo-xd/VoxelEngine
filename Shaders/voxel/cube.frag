#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in float AO;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

uniform DirectionalLight dirLight;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = dirLight.ambient * dirLight.color;
    vec3 diffuse = dirLight.diffuse * diff * dirLight.color;
    vec3 specular = dirLight.specular * spec * dirLight.color;

    vec3 lighting = (ambient + diffuse + specular * 0.3);

    vec3 result = Color * lighting * AO;

    float distance = length(viewPos - FragPos);
    float fogStart = 20.0;
    float fogEnd = 32.0;
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);
    vec3 fogColor = vec3(0.7, 0.85, 0.95);

    result = mix(fogColor, result, fogFactor);

    FragColor = vec4(result, 1.0);
}

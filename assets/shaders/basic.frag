#version 330 core

// Output color
out vec4 FragColor;

// Input from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    // Texture samplers
    sampler2D diffuse_map;
    sampler2D specular_map;
    bool use_texture;
};

// Light properties
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
    // Get base color from texture or material
    vec3 base_color;
    if (material.use_texture) {
        base_color = texture(material.diffuse_map, TexCoord).rgb;
    } else {
        base_color = material.diffuse;
    }

    // Ambient lighting
    vec3 ambient = light.ambient * material.ambient * base_color;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * base_color;

    // Specular lighting (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);




    vec3 specular_color;
    if (material.use_texture) {
        specular_color = texture(material.specular_map, TexCoord).rgb;
    } else {
        specular_color = material.specular;
    }
    vec3 specular = light.specular * spec * specular_color;

    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);  // Keep this line
}
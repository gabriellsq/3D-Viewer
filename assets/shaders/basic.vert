#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Vertex normal
layout (location = 2) in vec2 aTexCoord;  // Texture coordinates

// Output to fragment shader
out vec3 FragPos;    // World position of fragment
out vec3 Normal;     // Normal vector
out vec2 TexCoord;   // Texture coordinate

// Transformation matrices
uniform mat4 model;      // Model matrix (object to world)
uniform mat4 view;       // View matrix (world to camera)
uniform mat4 projection; // Projection matrix (camera to screen)

void main() {
    // Calculate world position
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal to world space
    // Note: We use the normal matrix to handle non-uniform scaling
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass texture coordinates unchanged
    TexCoord = aTexCoord;

    // Final vertex position in screen space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
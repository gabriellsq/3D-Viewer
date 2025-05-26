#!/bin/bash
# Complete 3D Viewer Project Setup Script

echo "Setting up 3D Viewer project structure..."

# Create main project directory
mkdir -p 3DViewer
cd 3DViewer

# Create all subdirectories
mkdir -p {assets/{models,shaders,textures},build,include,src,third_party}

echo "✓ Directory structure created"

# Create CMakeLists.txt (you already have this)
echo "✓ CMakeLists.txt ready"

# Create header files
echo "Creating header files..."

cat > include/Camera.h << 'EOF'
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    // Camera attributes
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 right;
    glm::vec3 world_up{0.0f, 1.0f, 0.0f};

    // Euler angles
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Camera options
    float movement_speed = 2.5f;
    float mouse_sensitivity = 0.1f;
    float fov = 45.0f;

    // Constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    // Returns view matrix
    glm::mat4 get_view_matrix() const;

    // Process input
    void process_keyboard(int direction, float delta_time);
    void process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch = true);
    void process_mouse_scroll(float y_offset);

private:
    void update_camera_vectors();
};
EOF

cat > include/Shader.h << 'EOF'
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    GLuint id;

    // Constructor
    Shader() = default;
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path);

    // Destructor
    ~Shader();

    // Move constructor and assignment
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Delete copy constructor and assignment
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Use/activate the shader
    void use() const;

    // Utility uniform functions
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec2(const std::string& name, const glm::vec2& value) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_mat2(const std::string& name, const glm::mat2& mat) const;
    void set_mat3(const std::string& name, const glm::mat3& mat) const;
    void set_mat4(const std::string& name, const glm::mat4& mat) const;

private:
    mutable std::unordered_map<std::string, GLint> uniform_cache;

    // Utility functions
    std::string load_shader_source(const std::string& path) const;
    GLuint compile_shader(const std::string& source, GLenum type) const;
    void check_compile_errors(GLuint shader, const std::string& type) const;
    GLint get_uniform_location(const std::string& name) const;
};
EOF

echo "✓ Header files created"

# You already have the implementation files (Camera.cpp and Shader.cpp)
echo "✓ Implementation files ready"

# Create basic shaders
echo "Creating shader examples..."

cat > assets/shaders/basic.vert << 'EOF'
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
EOF

cat > assets/shaders/basic.frag << 'EOF'
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
EOF

echo "✓ Basic shaders created"

# Create simple main.cpp
cat > src/main.cpp << 'EOF'
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "Shader.h"

#include <iostream>

// Settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

// Timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Viewer", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);

    // Build shader
    Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    // Cube vertices
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // You'll need to download this header

// Settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;
bool mouse_pressed = false;

// Timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// UI state
bool show_wireframe = false;
int current_object = 0;  // 0 = cube, 1 = pyramid
int current_shader = 0;  // 0 = basic lighting, 1 = simple color
glm::vec3 object_color(0.8f, 0.3f, 0.3f);
glm::vec3 light_color(2.0f, 2.0f, 2.0f);
glm::vec3 light_position(1.2f, 1.0f, 2.0f);
glm::vec3 object_rotation(0.0f, 0.0f, 0.0f);
bool auto_rotate = false;
float rotation_speed = 1.0f;  // Degrees per second


// Function prototypes
void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    // Only process mouse movement when left button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!mouse_pressed) {
            // First time clicking
            last_x = x_pos;
            last_y = y_pos;
            mouse_pressed = true;
            return;
        }

        float x_offset = x_pos - last_x;
        float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

        last_x = x_pos;
        last_y = y_pos;

        camera.process_mouse_movement(x_offset, y_offset);
    } else {
        mouse_pressed = false;
    }
}
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    camera.process_mouse_scroll(y_offset);
};

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(RIGHT, delta_time);
}

unsigned int load_texture(const std::string& path);

// Cube vertices with positions, normals, and texture coordinates
float cube_vertices[] = {
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
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// Pyramid vertices
float pyramid_vertices[] = {
        // Base (y = -0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  // front left
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  // front right
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  // back right
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  // back right
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  // back left
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  // front left

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.4472f,  0.8944f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.4472f,  0.8944f,  1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,  0.0f,  0.4472f,  0.8944f,  0.5f, 1.0f,

        // Right face
        0.5f, -0.5f,  0.5f,  0.8944f,  0.4472f,  0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.8944f,  0.4472f,  0.0f,  1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,  0.8944f,  0.4472f,  0.0f,  0.5f, 1.0f,

        // Back face
        0.5f, -0.5f, -0.5f,  0.0f,  0.4472f, -0.8944f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.4472f, -0.8944f,  1.0f, 0.0f,
        0.0f,  0.5f,  0.0f,  0.0f,  0.4472f, -0.8944f,  0.5f, 1.0f,

        // Left face
        -0.5f, -0.5f, -0.5f, -0.8944f,  0.4472f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -0.8944f,  0.4472f,  0.0f,  1.0f, 0.0f,
        0.0f,  0.5f,  0.0f, -0.8944f,  0.4472f,  0.0f,  0.5f, 1.0f
};

unsigned int create_test_texture() {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Create a simple 2x2 checkered pattern
    unsigned char data[] = {
            255, 0, 0, 255,    // Red
            0, 255, 0, 255,    // Green
            0, 0, 255, 255,    // Blue
            255, 255, 0, 255   // Yellow
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}




int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Basics", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    Shader basic_shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    Shader lighting_shader("assets/shaders/lighting.vert", "assets/shaders/lighting.frag"); // ToDo create a proper lighting shader

    // Set up vertex data for cube
    unsigned int cube_VAO, cube_VBO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);

    glBindVertexArray(cube_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Set up vertex data for pyramid
    unsigned int pyramid_VAO, pyramid_VBO;
    glGenVertexArrays(1, &pyramid_VAO);
    glGenBuffers(1, &pyramid_VBO);

    glBindVertexArray(pyramid_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramid_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texture1 = create_test_texture();

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Per-frame time logic

        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Input
        process_input(window);

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create ImGui controls
        ImGui::Begin("3D Controls");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Checkbox("Wireframe", &show_wireframe);
        ImGui::Combo("Object", &current_object, "Cube\0Pyramid\0");
        ImGui::Combo("Shader", &current_shader, "Basic\0Lighting\0");

        ImGui::ColorEdit3("Object Color", &object_color.x);
        ImGui::ColorEdit3("Light Color", &light_color.x);
        ImGui::SliderFloat3("Light Position", &light_position.x, -5.0f, 5.0f);

// Add rotation controls
        ImGui::Separator();
        ImGui::Text("Object Rotation");
        ImGui::SliderFloat3("Rotation (degrees)", &object_rotation.x, -180.0f, 180.0f);
        ImGui::Checkbox("Auto Rotate", &auto_rotate);
        if (auto_rotate) {
            ImGui::SliderFloat("Rotation Speed", &rotation_speed, 10.0f, 200.0f);
        }

        ImGui::End();

        // Set wireframe mode
        if (show_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Choose shader
        Shader* current_shader_ptr = (current_shader == 0) ? &basic_shader : &lighting_shader;
        current_shader_ptr->use();
        // Debug: Print which shader and try to get uniform locations
        // std::cout << "Using shader: " << (current_shader == 0 ? "basic" : "lighting") << std::endl;

// View/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 model = glm::mat4(1.0f);

        // Apply auto-rotation if enabled
        if (auto_rotate) {
            object_rotation.y += rotation_speed * delta_time;
            if (object_rotation.y > 360.0f) object_rotation.y -= 360.0f;
        }
        // Apply rotations to model matrix
        model = glm::rotate(model, glm::radians(object_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(object_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(object_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));


        // Only set uniforms that exist in the current shader
        if (current_shader == 0) {  // Basic shader
            current_shader_ptr->set_vec3("objectColor", object_color);
        } else {  // Lighting shader
            current_shader_ptr->set_vec3("objectColor", object_color);
            current_shader_ptr->set_vec3("lightColor", light_color);
            current_shader_ptr->set_vec3("lightPos", light_position);
            current_shader_ptr->set_vec3("viewPos", camera.position);
        }



        current_shader_ptr->set_mat4("projection", projection);
        current_shader_ptr->set_mat4("view", view);

        current_shader_ptr->set_mat4("model", model);




        current_shader_ptr->set_mat4("projection", projection);
        current_shader_ptr->set_mat4("view", view);
        current_shader_ptr->set_mat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        current_shader_ptr->set_int("texture1", 0);


        // Render the chosen object
        if (current_object == 0) {
            glBindVertexArray(cube_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        } else {
            glBindVertexArray(pyramid_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 18);
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteBuffers(1, &cube_VBO);
    glDeleteVertexArrays(1, &pyramid_VAO);
    glDeleteBuffers(1, &pyramid_VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
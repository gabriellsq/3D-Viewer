
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

// Movement directions
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position(position), world_up(up), yaw(yaw), pitch(pitch) {
    update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::process_keyboard(int direction, float delta_time) {
    float velocity = movement_speed * delta_time;

    switch (direction) {
        case FORWARD:
            position += front * velocity;
            break;
        case BACKWARD:
            position -= front * velocity;
            break;
        case LEFT:
            position -= right * velocity;
            break;
        case RIGHT:
            position += right * velocity;
            break;
        case UP:
            position += up * velocity;
            break;
        case DOWN:
            position -= up * velocity;
            break;
    }
}

void Camera::process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch) {
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update front, right and up vectors using the updated Euler angles
    update_camera_vectors();
}

void Camera::process_mouse_scroll(float y_offset) {
    fov -= y_offset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::update_camera_vectors() {
    // Calculate the new front vector
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(new_front);

    // Also re-calculate the right and up vector
    // Normalize the vectors, because their length gets closer to 0 the more 
    // you look up or down which results in slower movement.
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
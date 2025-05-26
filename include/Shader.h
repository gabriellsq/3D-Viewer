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
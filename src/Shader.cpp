#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    // Load shader source code
    std::string vertex_code = load_shader_source(vertex_path);
    std::string fragment_code = load_shader_source(fragment_path);

    // Compile shaders
    GLuint vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_code, GL_FRAGMENT_SHADER);

    // Create shader program
    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    // Check for linking errors
    check_compile_errors(id, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path) {
    // Load shader source code
    std::string vertex_code = load_shader_source(vertex_path);
    std::string fragment_code = load_shader_source(fragment_path);
    std::string geometry_code = load_shader_source(geometry_path);

    // Compile shaders
    GLuint vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_code, GL_FRAGMENT_SHADER);
    GLuint geometry_shader = compile_shader(geometry_code, GL_GEOMETRY_SHADER);

    // Create shader program
    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glAttachShader(id, geometry_shader);
    glLinkProgram(id);

    // Check for linking errors
    check_compile_errors(id, "PROGRAM");

    // Delete the shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(geometry_shader);
}

Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

Shader::Shader(Shader&& other) noexcept : id(other.id), uniform_cache(std::move(other.uniform_cache)) {
    other.id = 0; // Prevent the moved-from object from deleting the program
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        // Delete current program if it exists
        if (id != 0) {
            glDeleteProgram(id);
        }

        // Move resources
        id = other.id;
        uniform_cache = std::move(other.uniform_cache);

        // Prevent moved-from object from deleting the program
        other.id = 0;
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::set_bool(const std::string& name, bool value) const {
    glUniform1i(get_uniform_location(name), static_cast<int>(value));
}

void Shader::set_int(const std::string& name, int value) const {
    glUniform1i(get_uniform_location(name), value);
}

void Shader::set_float(const std::string& name, float value) const {
    glUniform1f(get_uniform_location(name), value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(get_uniform_location(name), 1, &value[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) const {
    glUniform2f(get_uniform_location(name), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
    GLint location = glGetUniformLocation(id, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(get_uniform_location(name), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(get_uniform_location(name), 1, &value[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(get_uniform_location(name), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}

std::string Shader::load_shader_source(const std::string& path) const {
    std::string shader_code;
    std::ifstream shader_file;

    // Ensure ifstream objects can throw exceptions
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open files
        shader_file.open(path);
        std::stringstream shader_stream;

        // Read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();

        // Close file handlers
        shader_file.close();

        // Convert stream into string
        shader_code = shader_stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::cerr << "Failed to load shader: " << path << std::endl;

        // Return a basic fallback shader based on the path
        if (path.find("vert") != std::string::npos || path.find(".vert") != std::string::npos) {
            // Basic vertex shader fallback
            return R"(
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
            )";
        } else {
            // Basic fragment shader fallback
            return R"(
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
            )";
        }
    }

    return shader_code;
}

GLuint Shader::compile_shader(const std::string& source, GLenum type) const {
    GLuint shader = glCreateShader(type);
    const char* source_cstr = source.c_str();
    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    std::string type_string;
    if (type == GL_VERTEX_SHADER)
        type_string = "VERTEX";
    else if (type == GL_FRAGMENT_SHADER)
        type_string = "FRAGMENT";
    else if (type == GL_GEOMETRY_SHADER)
        type_string = "GEOMETRY";

    check_compile_errors(shader, type_string);

    return shader;
}

void Shader::check_compile_errors(GLuint shader, const std::string& type) const {
    GLint success;
    GLchar info_log[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << info_log << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info_log);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << info_log << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}

GLint Shader::get_uniform_location(const std::string& name) const {
    // Check if location is already cached
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end()) {
        return it->second;
    }

    // Get location and cache it
    GLint location = glGetUniformLocation(id, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or is not used!" << std::endl;
    }

    uniform_cache[name] = location;
    return location;
}
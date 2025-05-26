cmake_minimum_required(VERSION 3.20)
project(3dBasics VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ==================== MULTIPLATFORM SETUP ====================

# Detect platform and architecture
if(APPLE)
    set(PLATFORM_NAME "macOS")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(ARCH_NAME "Apple Silicon")
        # Set Homebrew prefix for Apple Silicon
        set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})
        list(APPEND CMAKE_FRAMEWORK_PATH "/opt/homebrew/Frameworks")
    else()
        set(ARCH_NAME "Intel")
        # Set Homebrew prefix for Intel Mac
        set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
        list(APPEND CMAKE_FRAMEWORK_PATH "/usr/local/Frameworks")
    endif()
elseif(WIN32)
    set(PLATFORM_NAME "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_NAME "x64")
    else()
        set(ARCH_NAME "x86")
    endif()
    # Windows-specific paths for vcpkg or manual installations
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    endif()
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(ARCH_NAME "x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm.*")
        set(ARCH_NAME "ARM")
    else()
        set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
    endif()
endif()

# ==================== COMPILER OPTIONS ====================

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g)
    else()
        add_compile_options(-O3)
    endif()
endif()

# ==================== FIND PACKAGES ====================

# Find OpenGL (available on all platforms)
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)
if(NOT glfw3_FOUND)
    if(APPLE)
        message(FATAL_ERROR "GLFW not found. Install with: brew install glfw")
    elseif(WIN32)
        message(FATAL_ERROR "GLFW not found. Install with vcpkg: vcpkg install glfw3")
    else()
        message(FATAL_ERROR "GLFW not found. Install with: sudo apt-get install libglfw3-dev")
    endif()
endif()

# Find GLEW
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    if(APPLE)
        message(FATAL_ERROR "GLEW not found. Install with: brew install glew")
    elseif(WIN32)
        message(FATAL_ERROR "GLEW not found. Install with vcpkg: vcpkg install glew")
    else()
        message(FATAL_ERROR "GLEW not found. Install with: sudo apt-get install libglew-dev")
    endif()
endif()

# Find GLM
find_package(glm REQUIRED)
if(NOT glm_FOUND)
    if(APPLE)
        message(FATAL_ERROR "GLM not found. Install with: brew install glm")
    elseif(WIN32)
        message(FATAL_ERROR "GLM not found. Install with vcpkg: vcpkg install glm")
    else()
        message(FATAL_ERROR "GLM not found. Install with: sudo apt-get install libglm-dev")
    endif()
endif()

# ==================== IMGUI CONFIGURATION ====================

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

# Check if ImGui exists
if(NOT EXISTS ${IMGUI_DIR}/imgui.h)
    message(FATAL_ERROR
            "ImGui not found in ${IMGUI_DIR}. Please run:\n"
            "  mkdir -p third_party && cd third_party\n"
            "  git clone https://github.com/ocornut/imgui.git"
    )
endif()

# ImGui source files - only include what we need for OpenGL
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ==================== PROJECT FILES ====================

# Project source files (excluding imgui backends - we'll add them manually)
file(GLOB_RECURSE PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

# Only include main.cpp from root if it exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
endif()

# Remove any accidentally included imgui backend files and CMake files
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*imgui.*")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*CMakeFiles.*")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*cmake-build.*")

# Project header files
file(GLOB_RECURSE PROJECT_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

# ==================== CREATE EXECUTABLE ====================

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${IMGUI_SOURCES}
)

# ==================== INCLUDE DIRECTORIES ====================

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${OPENGL_INCLUDE_DIRS}
)

# ==================== LINK LIBRARIES ====================

# Base libraries for all platforms
target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glfw
        GLEW::GLEW
        glm::glm
)

# ==================== PLATFORM-SPECIFIC CONFIGURATIONS ====================

if(APPLE)
    # macOS-specific frameworks
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )

    # Silence OpenGL deprecation warnings on macOS
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            GL_SILENCE_DEPRECATION
    )

    # macOS specific compiler flags
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wno-deprecated-declarations
    )

elseif(WIN32)
    # Windows-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
            opengl32
            gdi32
            user32
            kernel32
    )

    # Windows-specific definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    # Set subsystem for Windows
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(${PROJECT_NAME} PROPERTIES
                WIN32_EXECUTABLE TRUE
        )
    endif()

elseif(UNIX)
    # Linux-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
            GL
            X11
            pthread
            Xrandr
            Xi
            dl
            m
    )

    # Linux-specific definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            LINUX
    )
endif()

# ==================== ASSET MANAGEMENT ====================

# Create asset directories in build folder
set(ASSET_DIRS shaders models textures)
foreach(ASSET_DIR ${ASSET_DIRS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/${ASSET_DIR})
endforeach()

# Copy assets to build directory (cross-platform)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    add_custom_target(copy_assets ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
            COMMENT "Copying assets to build directory"
    )
    add_dependencies(${PROJECT_NAME} copy_assets)
endif()

# ==================== INSTALLATION ====================

# Install target
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .  # For macOS app bundles
)

# Install assets
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    install(DIRECTORY assets/
            DESTINATION share/${PROJECT_NAME}/assets
    )
endif()

# ==================== CONFIGURATION SUMMARY ====================

message(STATUS "")
message(STATUS "=== ${PROJECT_NAME} Configuration ===")
message(STATUS "Platform: ${PLATFORM_NAME} (${ARCH_NAME})")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")
if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    message(STATUS "Homebrew prefix: /opt/homebrew")
elseif(APPLE)
    message(STATUS "Homebrew prefix: /usr/local")
endif()
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  OpenGL: ${OPENGL_LIBRARIES}")
message(STATUS "  GLFW: Found")
message(STATUS "  GLEW: Found")
message(STATUS "  GLM: Found")
message(STATUS "  ImGui: ${IMGUI_DIR}")
message(STATUS "")

# ==================== DEVELOPMENT HELPERS ====================

# Add custom targets for development
add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/assets
        COMMENT "Cleaning build directory and assets"
)

# Platform-specific run target
if(WIN32)
    add_custom_target(run
            COMMAND ${PROJECT_NAME}.exe
            DEPENDS ${PROJECT_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Running ${PROJECT_NAME}"
    )
else()
    add_custom_target(run
            COMMAND ./${PROJECT_NAME}
            DEPENDS ${PROJECT_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Running ${PROJECT_NAME}"
    )
endif()
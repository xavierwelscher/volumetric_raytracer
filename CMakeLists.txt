cmake_minimum_required(VERSION 3.24...3.31)

project(
    CaustX
    VERSION 0.1.0
    DESCRIPTION "Volumetric Raytracing Engine"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

find_package(OpenGL REQUIRED)
find_package(OpenMP)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)

# GLAD v2
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glfw glm imgui glad)

glad_add_library(glad_gl REPRODUCIBLE API gl:core=4.3)



add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC
    glfw
    glad_gl
    OpenGL::GL
)



add_executable(caustx
    src/main.cpp
		src/Engine.cpp
		src/Shader.cpp
)

target_include_directories(caustx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(caustx PRIVATE /W4 /O2 /arch:AVX2)
else()
    target_compile_options(caustx PRIVATE -Wall -Wextra -O3 -march=native)
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(caustx PRIVATE OpenMP::OpenMP_CXX)
endif()

target_link_libraries(caustx PRIVATE
    imgui_lib
    glm::glm
    glad_gl
    glfw
    OpenGL::GL
)

target_compile_definitions(caustx PRIVATE
				"SHADER_DIR=\"${CMAKE_SORUCE_DIR}/shaders\""
)

add_custom_command(TARGET caustx POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${CMAKE_SOURCE_DIR}/shaders"
				"$<TARGET_FILE_DIR:caustx>"
)

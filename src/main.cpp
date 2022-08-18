#include <cctype>
#include <glm/ext/matrix_transform.hpp>
#include <string>
#define _ _
#include <glad/glad.h>
#define __ __
#include <GLFW/glfw3.h>

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "mesh.hpp"
#include "util.hpp"
#include "vertex.hpp"

struct InputState {
    bool up, down, left, right;
    glm::vec2 last_mouse, mouse, mouse_delta;
    void update() {
        mouse_delta = mouse - last_mouse;
        last_mouse = mouse;
    }
} input_state;

const float PI = 3.1415926536;

const glm::vec2 SCREEN_SIZE = {1200, 800};

struct Player {
    glm::vec3 position;
    float pitch, yaw;
    float speed;
    float sensitivity;
    Player() {
        position = {0, 0.75, 10};
        pitch = yaw = 0;
        speed = 10;
        sensitivity = 0.01f;
    }
    glm::mat4 get_view_mat() {
        glm::mat4 view(1.0f);
        view = glm::rotate(view, -pitch, {1, 0, 0});
        view = glm::rotate(view, -yaw, {0, 1, 0});
        view = glm::translate(view, -position);
        return view;
    }
    void update(float dt) {
        glm::vec3 fd = {glm::sin(yaw), 0, glm::cos(yaw)};
        glm::vec3 rt = glm::cross({0, 1, 0}, fd);
        if (input_state.up) position -= fd * speed * dt;
        if (input_state.down) position += fd * speed * dt;
        if (input_state.left) position -= rt * speed * dt;
        if (input_state.right) position += rt * speed * dt;
        yaw -= input_state.mouse_delta.x * sensitivity;
        pitch -= input_state.mouse_delta.y * sensitivity;
        pitch = glm::clamp(pitch, -PI / 4, PI / 4);
    }
} player;

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    if (action == GLFW_REPEAT) return;
    bool pressed = action == GLFW_PRESS;
    switch (key) {
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
            input_state.up = pressed;
            break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            input_state.down = pressed;
            break;
        case GLFW_KEY_A:
        case GLFW_KEY_LEFT:
            input_state.left = pressed;
            break;
        case GLFW_KEY_D:
        case GLFW_KEY_RIGHT:
            input_state.right = pressed;
            break;
    }
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    input_state.mouse.x = xpos;
    input_state.mouse.y = ypos;
}

void error_callback(int code, const char* msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}

int main() {
    // Starting
    std::cout << "[INFO] Starting..." << std::endl;

    GLFWwindow* window;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to init GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    // Set window hints for opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Set window hints for window placement
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    window =
        glfwCreateWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "Hello", NULL, NULL);

    if (!window) {
        glfwTerminate();
        std::cerr << "[ERROR] Failed to create a window" << std::endl;
        exit(EXIT_FAILURE);
    }

    // input settings
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    // Set created window as current context
    glfwMakeContextCurrent(window);

    // Load gl procs
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "[ERROR] Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // Print out some info about renderer
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Load shaders
    std::cout << "[INFO] Loading shaders..." << std::endl;

    std::string vertex_shader = load_whole_file("shaders/standard.vs");
    std::string uvcolor_fragment_shader = load_whole_file("shaders/uvcolor.fs");
    std::string gray_frament_shader = load_whole_file("shaders/gray.fs");
    std::string textured_frament_shader =
        load_whole_file("shaders/textured.fs");

    std::cout << "[INFO] Finished loading shaders!" << std::endl;

    std::cout << "[INFO] Loading object..." << std::endl;

    std::vector<Vertex> voxel_verticies =
        parse_obj_format(load_whole_file("assets/wand.obj"));

    std::cout << "[INFO] Finished loading objects!" << std::endl;

    // Creating shader program
    GLuint uvcolor_glprog =
        create_shader_program(vertex_shader, uvcolor_fragment_shader);
    GLuint gray_glprog =
        create_shader_program(vertex_shader, gray_frament_shader);
    GLuint textured_glprog =
        create_shader_program(vertex_shader, textured_frament_shader);

    // Load palette texture for textured
    GLuint palette_texture = load_texture_file("assets/wand.png");
    GLuint tex_uniform_id = glGetUniformLocation(textured_glprog, "tex");
    glUniform1ui(tex_uniform_id, palette_texture);

    // Create quad mesh
    Mesh quad_mesh = Mesh::create_quad({-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
                                       {1.0f, -1.0f, 0.0f},
                                       {-1.0f, -1.0f, 0.0f}, uvcolor_glprog);
    // Create floor mesh
    Mesh floor_mesh = Mesh::create_quad({-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
                                        {1.0f, 0.0f, -1.0f},
                                        {-1.0f, 0.0f, -1.0f}, gray_glprog);

    /* floor_mesh.model = glm::translate(floor_mesh.model, {0, -1, 0}); */
    floor_mesh.model = glm::scale(floor_mesh.model, glm::vec3(100));

    // Create cube mesh
    Mesh cube_mesh = Mesh::create_cube({3, 0.5, 0}, 1, uvcolor_glprog);

    // Create voxel mesh
    Mesh voxel_mesh = Mesh(voxel_verticies, textured_glprog);

    // Set clear color
    glClearColor(0.7f, 0.7f, 0.7f, 1.f);
    std::chrono::high_resolution_clock::time_point last_time =
        std::chrono::high_resolution_clock::now();

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    float time_since_last_fps_count = 0;
    int frames = 0;
    float rotation = 0;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // dt calculation
        std::chrono::high_resolution_clock::time_point now_time =
            std::chrono::high_resolution_clock::now();
        float dt =
            std::chrono::duration<float, std::milli>(now_time - last_time)
                .count() /
            1000;
        last_time = now_time;

        // fps display
        frames++;
        time_since_last_fps_count += dt;
        if (time_since_last_fps_count >= 3) {
            char buff[255];
            float fps = frames / time_since_last_fps_count;
            sprintf(buff, "Hello (fps: %.1f)", fps);
            glfwSetWindowTitle(window, buff);
            time_since_last_fps_count = 0;
            frames = 0;
        }

        // input
        input_state.update();

        // updating
        player.update(dt);

        rotation += PI * dt;
        voxel_mesh.model = glm::rotate(glm::mat4(1), rotation, {0, 1, 0});

        // screen clearing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering
        glm::mat4 projection =
            glm::perspective(45.f, SCREEN_SIZE.x / SCREEN_SIZE.y, 0.1f, 1000.f);

        glm::mat4 view = player.get_view_mat();

        floor_mesh.render(view, projection);
        /* quad_mesh.render(view, projection); */
        cube_mesh.render(view, projection);
        voxel_mesh.render(view, projection);

        // glfw things after render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Exiting
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "[INFO] Exiting gracefully" << std::endl;
    exit(EXIT_SUCCESS);
}

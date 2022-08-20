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

const float SUN_VIEW_SIZE = 8;
const int SUN_TEX_SIZE = 1024;

const glm::mat4 PLAYER_CAMERA_PROJECTION =
    glm::perspective(45.f, SCREEN_SIZE.x / SCREEN_SIZE.y, 0.01f, 1000.f);

struct Camera {
    // transformation thingies
    glm::vec3 position;
    float pitch, yaw;

    // projection
    glm::mat4 projection;

    // movement thingies
    float speed;
    float sensitivity;

    // framebuffer thingies
    GLuint fbo;
    GLuint color_tex;
    GLuint depth_tex;
    unsigned int view_w, view_h;

    Camera(glm::mat4 projection, unsigned int w = SCREEN_SIZE.x,
           unsigned int h = SCREEN_SIZE.y)
        : projection(projection), view_w(w), view_h(h) {
        position = {0, 1, 10};
        pitch = yaw = 0;
        speed = 10;
        sensitivity = 0.01f;
    }

    void init_fbo() {
        // create framebuffer object
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // create color texture
        glGenTextures(1, &color_tex);
        glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view_w, view_h, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, color_tex, 0);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
               GL_FRAMEBUFFER_COMPLETE);

        // create depth buffer
        glGenTextures(1, &depth_tex);
        glBindTexture(GL_TEXTURE_2D, depth_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, view_w, view_h, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depth_tex,
                               0);  // attach texture to 0
#if 0
        glGenRenderbuffers(1, &depth_tex);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_tex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, view_w,
                              view_h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depth_tex);  // attach depth buffer

#endif
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
               GL_FRAMEBUFFER_COMPLETE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bind_fbo() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, view_w, view_h);
    }

    void unbind_fbo() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

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

    ~Camera() {
        glDeleteTextures(1, &color_tex);
        glDeleteTextures(1, &depth_tex);
        /* glDeleteRenderbuffers(1, &depth_tex); */
        glDeleteFramebuffers(1, &fbo);
    }
} player_camera(PLAYER_CAMERA_PROJECTION);

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

    //////////////////////////////////////// GL PROCEDURES LOADED

    // Initialize player camera framebuffer
    player_camera.init_fbo();

    glm::mat4 sun_projection =
        glm::ortho(-SUN_VIEW_SIZE, SUN_VIEW_SIZE, -SUN_VIEW_SIZE, SUN_VIEW_SIZE,
                   0.0f, 1000.f);
    Camera sun(sun_projection, SUN_TEX_SIZE, SUN_TEX_SIZE);

    sun.init_fbo();

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
    std::string screen_frament_shader =
        load_whole_file("shaders/textured_unlit.fs");

    std::cout << "[INFO] Finished loading shaders!" << std::endl;

    // Creating shader program
    GLuint uvcolor_glprog =
        create_shader_program(vertex_shader, uvcolor_fragment_shader);
    GLuint gray_glprog =
        create_shader_program(vertex_shader, gray_frament_shader);
    GLuint textured_glprog =
        create_shader_program(vertex_shader, textured_frament_shader);
    GLuint screen_glprog =
        create_shader_program(vertex_shader, screen_frament_shader);

    // Load palette texture for textured
    GLuint palette_texture = load_texture_file("assets/wand.png");
    GLuint tex_uniform_id = glGetUniformLocation(textured_glprog, "tex");
    glUniform1ui(tex_uniform_id, palette_texture);

    // Create quad mesh
    Mesh quad_mesh = Mesh::create_quad({-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
                                       {1.0f, -1.0f, 0.0f},
                                       {-1.0f, -1.0f, 0.0f}, uvcolor_glprog);
    // Create floor mesh
    Mesh floor_mesh = Mesh::create_quad(
        {-10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, -10.0f},
        {-10.0f, 0.0f, -10.0f}, gray_glprog);

    /* floor_mesh.model = glm::translate(floor_mesh.model, {0, -1, 0}); */
    /* floor_mesh.model = glm::scale(floor_mesh.model, glm::vec3(100)); */

    // Create cube mesh
    Mesh cube_mesh = Mesh::create_cube({3, 0.5, 0}, 1, uvcolor_glprog);

    // Create voxel mesh
    Mesh wand_mesh = Mesh::create_from_obj("assets/wand.obj", textured_glprog,
                                           palette_texture);

    // Create shotgun mesh
    Mesh shotgun_mesh = Mesh::create_from_obj("assets/shotgun.obj",
                                              textured_glprog, palette_texture);
    // Create screen quad mesh
    Mesh screen_quad_mesh = Mesh::create_quad(
        {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f}, screen_glprog, player_camera.color_tex,
        player_camera.depth_tex);

    std::chrono::high_resolution_clock::time_point last_time =
        std::chrono::high_resolution_clock::now();

    float time_since_last_fps_count = 0;
    int frames = 0;
    float rotation = 0;

    // Enable face culling
    /* glEnable(GL_CULL_FACE); */

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
        player_camera.update(dt);

        sun.position = player_camera.position + glm::vec3{10, 10, 10};
        /* sun.position = player_camera.position + glm::vec3{0, 10, 0}; */
        sun.yaw = PI / 4;
        sun.pitch = -PI / 4;
        /* sun.pitch = -PI / 2; */

        rotation += PI * dt;
        wand_mesh.model = glm::rotate(glm::mat4(1), rotation, {0, 1, 0});

        shotgun_mesh.model =
            glm::translate(glm::mat4(1), player_camera.position);
        shotgun_mesh.model =
            glm::rotate(shotgun_mesh.model, player_camera.yaw, {0, 1, 0});
        shotgun_mesh.model =
            glm::rotate(shotgun_mesh.model, player_camera.pitch, {1, 0, 0});
        shotgun_mesh.model =
            glm::translate(shotgun_mesh.model, {0.8f, -0.6f, -1.f});
        shotgun_mesh.model =
            glm::rotate(shotgun_mesh.model, -PI / 2, {1, 0, 0});
        shotgun_mesh.model = glm::scale(shotgun_mesh.model, glm::vec3(0.8f));

        // rendering
        std::vector<Mesh*> normal_meshes_to_render = {&floor_mesh, &cube_mesh,
                                                      &wand_mesh};
        {  // sun camera rendering
            sun.bind_fbo();
            glClearColor(1.f, 0.7f, 0.7f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            glm::mat4 projection = sun.projection;
            glm::mat4 view = sun.get_view_mat();

            for (auto mesh : normal_meshes_to_render) {
                mesh->tex1 = 0;
                mesh->render(view, projection);
            }

            sun.unbind_fbo();
        }

        {  // main camera rendering
            player_camera.bind_fbo();
            glClearColor(0.7f, 0.7f, 0.7f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            glm::mat4 projection = player_camera.projection;
            glm::mat4 view = player_camera.get_view_mat();

            glm::mat4 sun_projection = sun.projection;
            glm::mat4 sun_view = sun.get_view_mat();

            for (auto mesh : normal_meshes_to_render) {
                mesh->tex1 = sun.depth_tex;
                mesh->render(view, projection, sun_view, sun_projection);
            }

            // clear depth buffer to draw always on top
            /* glClear(GL_DEPTH_BUFFER_BIT); */
            shotgun_mesh.render(view, projection);

            player_camera.unbind_fbo();
        }

        glDisable(GL_DEPTH_TEST);
        // render main camera framebuffer
        screen_quad_mesh.render(glm::mat4(1), glm::mat4(1));

        // glfw things after render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Exiting
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "[INFO] Exiting gracefully" << std::endl;
    exit(EXIT_SUCCESS);

    return 0;
}

#include "mesh.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <iostream>

#include "util.hpp"

Mesh::Mesh(std::vector<Vertex>& verticies, GLuint shader_prog, GLuint texture0,
           GLuint texture1)
    : prog(shader_prog),
      verticies(verticies),
      model(glm::mat4(1)),
      tex0(texture0),
      tex1(texture1) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex),
                 verticies.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GLFW_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GLFW_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texture_coord));
    glVertexAttribPointer(2, 3, GL_FLOAT, GLFW_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glBindVertexArray(0);

    modelID = glGetUniformLocation(prog, "model");
    assert(modelID >= 0);
    viewID = glGetUniformLocation(prog, "view");
    assert(viewID >= 0);
    projectionID = glGetUniformLocation(prog, "projection");
    assert(projectionID >= 0);
    normal_modelID = glGetUniformLocation(prog, "normal_model");
    assert(normal_modelID >= 0);

    sun_viewID = glGetUniformLocation(prog, "sun_view");
    assert(sun_viewID >= 0);
    sun_projectionID = glGetUniformLocation(prog, "sun_projection");
    assert(sun_projectionID >= 0);

    tex0_ID = glGetUniformLocation(prog, "tex0");
    glUniform1i(tex0_ID, 0);
    tex1_ID = glGetUniformLocation(prog, "tex1");
    glUniform1i(tex1_ID, 1);
}

Mesh Mesh::create_quad(glm::vec3 top_left, glm::vec3 top_right,
                       glm::vec3 bottom_right, glm::vec3 bottom_left,
                       GLuint shader_prog, GLuint texture0, GLuint texture1) {
    std::vector<Vertex> verticies = {
        {.position = top_left, .texture_coord = {0.0f, 1.0f}},
        {.position = bottom_left, .texture_coord = {0.0f, 0.0f}},
        {.position = top_right, .texture_coord = {1.0f, 1.0f}},
        {.position = top_right, .texture_coord = {1.0f, 1.0f}},
        {.position = bottom_left, .texture_coord = {0.0f, 0.0f}},
        {.position = bottom_right, .texture_coord = {1.0f, 0.0f}},
    };

    return Mesh(verticies, shader_prog, texture0, texture1);
}

Mesh Mesh::create_cube(glm::vec3 center, float a, GLuint shader_prog) {
    float ha = a * 0.5f;
    std::vector<Vertex> cube_verticies;

    // bottom
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{-1, -1, -1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {0, -1, 0}},
            {.position = center + glm::vec3{1, -1, -1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {0, -1, 0}},
            {.position = center + glm::vec3{1, -1, 1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {0, -1, 0}},
            {.position = center + glm::vec3{-1, -1, 1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {0, -1, 0}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    // top
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{-1, 1, 1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {0, 1, 0}},
            {.position = center + glm::vec3{1, 1, 1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {0, 1, 0}},
            {.position = center + glm::vec3{1, 1, -1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {0, 1, 0}},
            {.position = center + glm::vec3{-1, 1, -1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {0, 1, 0}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    // front
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{-1, 1, -1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {0, 0, -1}},
            {.position = center + glm::vec3{1, 1, -1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {0, 0, -1}},
            {.position = center + glm::vec3{1, -1, -1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {0, 0, -1}},
            {.position = center + glm::vec3{-1, -1, -1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {0, 0, -1}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    // back
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{-1, -1, 1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {0, 0, 1}},
            {.position = center + glm::vec3{1, -1, 1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {0, 0, 1}},
            {.position = center + glm::vec3{1, 1, 1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {0, 0, 1}},
            {.position = center + glm::vec3{-1, 1, 1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {0, 0, 1}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    // right
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{1, 1, -1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {1, 0, 0}},
            {.position = center + glm::vec3{1, 1, 1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {1, 0, 0}},
            {.position = center + glm::vec3{1, -1, 1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {1, 0, 0}},
            {.position = center + glm::vec3{1, -1, -1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {1, 0, 0}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    // left
    {
        std::vector<Vertex> tmp = {
            {.position = center + glm::vec3{-1, -1, -1} * ha,
             .texture_coord = {1.0f, 0.0f},
             .normal = {-1, 0, 0}},
            {.position = center + glm::vec3{-1, -1, 1} * ha,
             .texture_coord = {0.0f, 0.0f},
             .normal = {-1, 0, 0}},
            {.position = center + glm::vec3{-1, 1, 1} * ha,
             .texture_coord = {0.0f, 1.0f},
             .normal = {-1, 0, 0}},
            {.position = center + glm::vec3{-1, 1, -1} * ha,
             .texture_coord = {1.0f, 1.0f},
             .normal = {-1, 0, 0}},
        };
        tmp = quad_of_four_verticies(tmp);
        cube_verticies.insert(cube_verticies.end(), tmp.begin(), tmp.end());
    }

    return Mesh(cube_verticies, shader_prog);
}

Mesh Mesh::create_from_obj(const char* filename, GLuint shader_prog,
                           GLuint texture0, GLuint texture1) {
    std::vector<Vertex> verticies = parse_obj_format(load_whole_file(filename));
    return Mesh(verticies, shader_prog, texture0, texture1);
}

void Mesh::render(glm::mat4 view, glm::mat4 projection, glm::mat4 sun_view, glm::mat4 sun_projection) {
    // Use shader program
    glUseProgram(prog);
    // rendering
    glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(sun_viewID, 1, GL_FALSE, glm::value_ptr(sun_view));
    glUniformMatrix4fv(sun_projectionID, 1, GL_FALSE, glm::value_ptr(sun_projection));

    glm::mat3 normal_model = glm::transpose(glm::inverse(model));
    glUniformMatrix3fv(normal_modelID, 1, GL_FALSE,
                       glm::value_ptr(normal_model));
    if (tex0_ID && tex0) {
        glUniform1i(tex0_ID, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
    }
    if (tex1_ID && tex1) {
        glUniform1i(tex1_ID, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
    }

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, verticies.size());

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Mesh::set_uniform(const char* name, glm::mat4 m) {
    GLuint id = glad_glGetUniformLocation(prog, name);
    glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(m));
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

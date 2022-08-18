#ifndef __MESH_HPP
#define __MESH_HPP

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "vertex.hpp"

struct Mesh {
    std::vector<Vertex> verticies;
    glm::mat4 model;
    GLuint prog, vao, vbo;
    GLuint modelID, viewID, projectionID, normal_modelID;
    Mesh(std::vector<Vertex>& verticies, GLuint shader_prog);
    static Mesh create_quad(glm::vec3 top_left, glm::vec3 top_right,
                            glm::vec3 bottom_right, glm::vec3 bottom_left,
                            GLuint shader_prog);
    static Mesh create_cube(glm::vec3 center, float a, GLuint shader_prog);
    void render(glm::mat4 view, glm::mat4 projection);
    ~Mesh();
};

#endif  // __MESH_HPP


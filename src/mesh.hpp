#ifndef __MESH_HPP
#define __MESH_HPP

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "vertex.hpp"

struct Mesh {
    std::vector<Vertex> verticies;
    glm::mat4 model;
    GLuint prog, vao, vbo, tex0, tex1;
    GLuint modelID, viewID, projectionID, sun_viewID, sun_projectionID,
        normal_modelID, tex0_ID, tex1_ID;

    Mesh(std::vector<Vertex>& verticies, GLuint shader_prog,
         GLuint texture0 = 0, GLuint texture1 = 0);
    static Mesh create_quad(glm::vec3 top_left, glm::vec3 top_right,
                            glm::vec3 bottom_right, glm::vec3 bottom_left,
                            GLuint shader_prog, GLuint texture0 = 0,
                            GLuint texture1 = 0);
    static Mesh create_cube(glm::vec3 center, float a, GLuint shader_prog);
    static Mesh create_from_obj(const char* filename, GLuint shader_prog,
                                GLuint texture0 = 0, GLuint texture1 = 0);
    void render(glm::mat4 view, glm::mat4 projection,
                glm::mat4 sun_view = glm::mat4(0),
                glm::mat4 sun_projection = glm::mat4(0));
    void set_uniform(const char* name, glm::mat4 m);
    ~Mesh();
};

#endif  // __MESH_HPP


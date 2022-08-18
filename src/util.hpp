#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "vertex.hpp"

GLuint create_shader_program(const char* vert_src, const char* frag_src);
GLuint create_shader_program(std::string const& vert_src,
                             std::string const& frag_src);
void print_mat4(glm::mat4 const& m);
void print_vec4(glm::vec4 const& v);
std::string load_whole_file(const char* filename);
std::vector<Vertex> quad_of_four_verticies(std::vector<Vertex>& vs);

GLuint load_texture_file(const char* filename);
std::vector<Vertex> parse_obj_format(std::string const& input);

#endif  // __UTIL_HPP

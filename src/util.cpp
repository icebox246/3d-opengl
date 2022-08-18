#include "util.hpp"

#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

GLuint create_shader_program(std::string const& vert_src,
                             std::string const& frag_src) {
    return create_shader_program(vert_src.c_str(), frag_src.c_str());
}

GLuint create_shader_program(const char* vert_src, const char* frag_src) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    GLint res = GL_FALSE;

    int log_size = 0;

    // compile vertex shader
    glShaderSource(vs, 1, &vert_src, 0);
    glCompileShader(vs);

    // check vertex shader
    glGetShaderiv(vs, GL_COMPILE_STATUS, &res);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 1) {
        std::vector<char> errmsg(log_size + 1, 0);
        glGetShaderInfoLog(vs, log_size, 0, errmsg.data());
        std::cout << errmsg.data() << std::endl;
    }

    // compile fragment shader
    glShaderSource(fs, 1, &frag_src, 0);
    glCompileShader(fs);

    // check fragment shader
    glGetShaderiv(fs, GL_COMPILE_STATUS, &res);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 1) {
        std::vector<char> errmsg(log_size + 1, 0);
        glGetShaderInfoLog(fs, log_size, 0, errmsg.data());
        std::cout << errmsg.data() << std::endl;
    }

    // create and link program
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    // check program
    glGetProgramiv(prog, GL_LINK_STATUS, &res);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 1) {
        std::vector<char> errmsg(log_size + 1, 0);
        glGetProgramInfoLog(prog, log_size, 0, errmsg.data());
        std::cout << errmsg.data() << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

void print_mat4(glm::mat4 const& m) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            std::cout << m[x][y] << '\t';
        }
        std::cout << std::endl;
    }
}

void print_vec4(glm::vec4 const& v) {
    for (int x = 0; x < 4; x++) {
        std::cout << v[x] << '\t';
    }
    std::cout << std::endl;
}

std::string load_whole_file(const char* filename) {
    std::ifstream is(filename);

    is.seekg(0, is.end);
    size_t len = is.tellg();
    is.seekg(0, is.beg);

    char buffer[len + 1];
    for (size_t i = 0; i < len + 1; i++) buffer[i] = 0;

    is.read(buffer, len);
    is.close();

    return std::string(buffer);
}

std::vector<Vertex> quad_of_four_verticies(std::vector<Vertex>& vs) {
    assert(vs.size() == 4);
    return {vs[0], vs[1], vs[3], vs[1], vs[2], vs[3]};
}

GLuint load_texture_file(const char* filename) {
    int x, y, n;
    unsigned char* data = stbi_load(filename, &x, &y, &n, 4);
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << filename
                  << std::endl;
    }

    GLenum tex_format = GL_RGBA;
    if (n == 3) tex_format = GL_RGB;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

/*
 * Supports only:
 *  - 3d positions
 *  - 3d normals
 *  - 2d texture coords
 *  - triangular faces
 */
std::vector<Vertex> parse_obj_format(std::string const& input) {
    std::stringstream iss(input);
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<Vertex> output;
    int ln = 0;
    while (!iss.eof()) {  // parse line by line until eof
        ln++;
        std::string line;
        std::getline(iss, line);
        if (line.size() == 0 || line[0] == '#')  // comment or empty line
            continue;

        std::stringstream lss(line);
        std::string command;
        lss >> command;
        if (command.size() == 0) continue;  // empty line

        if (command == "v") {
            glm::vec3 v;
            lss >> v.x >> v.y >> v.z;
            positions.push_back(v);
        } else if (command == "vn") {
            glm::vec3 n;
            lss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (command == "vt") {
            glm::vec2 tc;
            lss >> tc.x >> tc.y;
            texcoords.push_back(tc);
        } else if (command == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertex_index;
                std::vector<int> indicies;
                lss >> vertex_index;
                {
                    int acc = 0;
                    int sig = 1;
                    for (int j = 0; j <= vertex_index.size(); j++) {
                        if (j == vertex_index.size() ||
                            vertex_index[j] == '/') {
                            indicies.push_back(acc * sig);
                            acc = 0;
                            sig = 1;
                        } else if (vertex_index[j] == '-') {
                            sig *= -1;
                        } else if (std::isdigit(vertex_index[j])) {
                            acc = acc * 10 + (vertex_index[j] - '0');
                        } else {
                            std::cerr << "[WARN] Unsupported character in "
                                         "vertex index: "
                                      << vertex_index[j] << std::endl;
                        }
                    }
                }

                if (indicies.size() == 0) {
                    std::cerr << "[ERROR] Invalid vertex index format in face "
                                 "definition in OBJ file on line: "
                              << ln << std::endl;
                    exit(1);
                }

                int pi, ti, ni;
                pi = indicies[0];
                ti = (indicies.size() > 1) ? indicies[1] : 0;
                ni = (indicies.size() > 2) ? indicies[2] : 0;

                Vertex result;
                if (pi > 0) result.position = positions[pi - 1];
                if (pi < 0) result.position = positions[positions.size() + pi];
                if (ti > 0) result.texture_coord = texcoords[ti - 1];
                if (ti < 0)
                    result.texture_coord = texcoords[texcoords.size() + ti];
                if (ni > 0) result.normal = normals[ni - 1];
                if (ni < 0) result.normal = normals[normals.size() + ni];

                output.push_back(result);
            }
        } else {
            std::cerr << "[WARN] OBJ parser does not support: " << line
                      << std::endl;
        }
    }

    return output;
}

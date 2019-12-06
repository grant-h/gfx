#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

#include <Shader.hpp>

class ShaderProgram {
  public:
    ShaderProgram();
    ~ShaderProgram();

    void add_shader(std::shared_ptr<Shader> shader);
    bool link();
    GLuint get_program_id();
  private:
    void release();

    std::vector<std::shared_ptr<Shader>> shaders_;
    GLuint program_id_;
    bool linked_;
};

#endif // _SHADER_PROGRAM_H

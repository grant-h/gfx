#ifndef _SHADER_H
#define _SHADER_H

#include <GLFW/glfw3.h>
#include <string>

class Shader {
  public:
    Shader(GLenum shader_type);
    ~Shader();

    std::string get_name();

    bool load_from_file(std::string filepath);
    bool reload();
    void set_code(std::string code);
    std::string & get_code() { return code_; }
    bool compile();
    GLuint get_shader_id();
  private:
    void release();

    std::string filepath_;
    std::string code_;

    GLuint shader_id_;
    GLenum shader_type_;
    bool compiled_;
};

#endif // _SHADER_H

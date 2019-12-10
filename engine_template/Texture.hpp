#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <Renderer.hpp>
#include <string>

class Texture {
  public:
    Texture(std::string name);
    ~Texture();

    std::string get_name();

    bool load_from_file(std::string filepath);
    int get_width();
    int get_height();
    int get_channels();
    GLenum get_texture_target();

    void use();
    void unuse();
  private:
    void release();

    std::string filepath_;
    std::string name_;
    GLuint texture_id_;
    GLenum texture_format_;
    GLenum texture_target_;
    int width_, height_, num_channels_;
};

#endif // _TEXTURE_H

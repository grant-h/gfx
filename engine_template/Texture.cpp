#include <Texture.hpp>

#include <Log.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(std::string name)
  :name_(name), texture_id_(0), width_(0), height_(0), num_channels_(0),
  texture_format_(0), texture_target_(0)
{
}

Texture::~Texture()
{
  release();
}

std::string Texture::get_name()
{
  return name_;
}

bool Texture::load_from_file(std::string filepath)
{
  release();

  // OpenGL x/y coordinate system fix
  stbi_set_flip_vertically_on_load(true);

  // request 4 channels for RGBA
  unsigned char * data = stbi_load(filepath.c_str(), &width_, &height_, &num_channels_, 4);

  if (!data) {
    LOG_ERROR("Failed to load texture %s from '%s': %s",
        name_.c_str(), filepath.c_str(), stbi_failure_reason());

    return false;
  }

  if (num_channels_ < 3) {
    LOG_ERROR("Failed to load texture %s from '%s': Non-RGB/RGBA textures unsupported",
        name_.c_str(), filepath.c_str());

    stbi_image_free(data);
    return false;
  }

  // TODO: dont always use alpha textures to save memory
  texture_format_ = GL_RGBA;
  texture_target_ = GL_TEXTURE_2D;

  glGenTextures(1, &texture_id_);
  use();

  glTexParameteri(texture_target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(texture_target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(texture_target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(texture_target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(texture_target_, 0, texture_format_, width_, height_,
      0, texture_format_, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(texture_target_);

  unuse();

  stbi_image_free(data);

  filepath_ = filepath;
  return true;
}

int Texture::get_width()
{
  return width_;
}

int Texture::get_height()
{
  return height_;
}

int Texture::get_channels()
{
  return num_channels_;
}

GLenum Texture::get_texture_target() {
  return texture_target_;
}

void Texture::use()
{
  glBindTexture(texture_target_, texture_id_);
}

void Texture::unuse()
{
  glBindTexture(texture_target_, 0);
}

void Texture::release()
{
  if (texture_id_ != 0)
    glDeleteTextures(1, &texture_id_);

  filepath_ = "";
  texture_id_ = 0;
  texture_format_ = 0;
  texture_target_ = 0;
  width_ = 0;
  height_ = 0;
  num_channels_ = 0;
}

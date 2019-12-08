#include "Shader.hpp"

#include <fstream>

#include <Log.hpp>

Shader::Shader(GLenum shader_type)
  :shader_id_(0), shader_type_(shader_type)
{
}

Shader::~Shader()
{
  release();
}

std::string Shader::get_name()
{
  return filepath_;
}

bool Shader::load_from_file(std::string filepath)
{
  std::ifstream stream(filepath, std::ios::in);

  LOG_INFO("Loading shader %s", filepath.c_str());
  if (stream.is_open()) {
    std::string line;
    std::string code;

    while (getline(stream, line))
      code += "\n" + line;

    stream.close();

    filepath_ = filepath;
    set_code(code);
  } else {
    LOG_ERROR("unable to open shader %s for reading", filepath.c_str());
    filepath_ = "";
    set_code("");
    return false;
  }

  return true;
}

bool Shader::reload()
{
  return load_from_file(filepath_);
}

void Shader::set_code(std::string code)
{
  release();
  code_ = code;
}

bool Shader::compile()
{
  if (compiled_)
    return true;

  GLuint shader_id = glCreateShader(shader_type_);

  if (shader_id == 0) {
    assert(glGetError() != GL_INVALID_ENUM);
    LOG_ERROR("unable to create shader: glCreateShader failed");
    return false;
  }

  LOG_DEBUG("Compiling shader: %s", filepath_.c_str());

  char const * source = code_.c_str();
  GLint result = GL_FALSE;
  GLint log_length = 0;

  glShaderSource(shader_id, 1, &source , NULL);
  glCompileShader(shader_id);

  // Check Shader
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

  if (result == GL_FALSE) {
    LOG_ERROR("Failed to compile shader");
  }

  if (log_length > 0) {
    char * log_buffer = new char[log_length];
    glGetShaderInfoLog(shader_id, log_length, NULL, log_buffer);

    LOG_ERROR("~~~~~ SHADER LOG~~~~~\n%s", log_buffer);
    delete [] log_buffer;
  }

  if (result == GL_FALSE) {
    glDeleteShader(shader_id);
    return false;
  }

  shader_id_ = shader_id;

  compiled_ = true;
  return true;
}

GLuint Shader::get_shader_id()
{
  return shader_id_;
}

void Shader::release()
{
  if (shader_id_ != 0)
    glDeleteShader(shader_id_);

  shader_id_ = 0;
  compiled_ = false;
}

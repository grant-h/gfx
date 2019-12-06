#include "ShaderProgram.hpp"

#include <Log.hpp>

ShaderProgram::ShaderProgram()
  :program_id_(0), linked_(false)
{
}

ShaderProgram::~ShaderProgram()
{
  release();
}

void ShaderProgram::add_shader(std::shared_ptr<Shader> shader)
{
  shaders_.push_back(std::move(shader));
}

bool ShaderProgram::link()
{
  release();

  std::string name_list;

  for(size_t i = 0; i < shaders_.size(); i++) {
    name_list += shaders_.at(i)->get_name();
    if (i+1 != shaders_.size())
      name_list += ", ";
  }

  for(size_t i = 0; i < shaders_.size(); i++) {
    if (!shaders_.at(i)->compile())
      return false;
  }

  LOG_DEBUG("Linking shader program <- [%s]", name_list.c_str());

  GLuint program_id = glCreateProgram();

  if (program_id == 0) {
    LOG_ERROR("unable to link shader: glCreateProgram failed");
    return false;
  }

  for(size_t i = 0; i < shaders_.size(); i++)
    glAttachShader(program_id, shaders_.at(i)->get_shader_id());

  GLint result = GL_FALSE;
  GLint log_length = 0;

  glLinkProgram(program_id);
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

  if (result == GL_FALSE) {
    LOG_ERROR("Failed to link shader program");
  }

  if (log_length > 0) {
    char * log_buffer = new char[log_length];
    glGetProgramInfoLog(program_id, log_length, NULL, log_buffer);

    LOG_ERROR("~~~~~ SHADER LINK LOG~~~~~\n%s", log_buffer);
    delete log_buffer;
  }

  if (result == GL_FALSE) {
    glDeleteProgram(program_id);
    return false;
  }

  for(size_t i = 0; i < shaders_.size(); i++)
    glDetachShader(program_id, shaders_.at(i)->get_shader_id());

  program_id_ = program_id;
  linked_ = true;

  return true;
}

GLuint ShaderProgram::get_program_id()
{
  return program_id_;
}

void ShaderProgram::release()
{
  if (program_id_ != 0)
    glDeleteProgram(program_id_);

  linked_ = false;
  program_id_ = 0;
}

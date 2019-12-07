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
    delete [] log_buffer;
  }

  if (result == GL_FALSE) {
    glDeleteProgram(program_id);
    return false;
  }

  for(size_t i = 0; i < shaders_.size(); i++)
    glDetachShader(program_id, shaders_.at(i)->get_shader_id());

  program_id_ = program_id;
  linked_ = true;

  refresh_uniforms();

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

void ShaderProgram::refresh_uniforms()
{
  if (!linked_)
    return;

  GLint count;

  uniform_map_.clear();

  glGetProgramiv(program_id_, GL_ACTIVE_UNIFORMS, &count);

  for (int i = 0; i < count; i++)
  {
      GLint size;
      GLenum type;
      const GLsizei bufSize = 256;
      GLchar name[bufSize];
      GLsizei length;

      glGetActiveUniform(program_id_, (GLuint)i, bufSize, &length, &size, &type, name);
      GLint location = glGetUniformLocation(program_id_, name);

      assert(location >= 0);

      assert(uniform_map_.find(name) == uniform_map_.end());
      uniform_map_[name] = std::pair<GLint,GLenum>(location, type);

      printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
  }
}

void ShaderProgram::refresh_attributes()
{
  GLint i;
  GLint count;

  GLint size; // size of the variable
  GLenum type; // type of the variable (float, vec3 or mat4, etc)

  const GLsizei bufSize = 256; // maximum name length
  GLchar name[bufSize]; // variable name in GLSL
  GLsizei length; // name length

  glGetProgramiv(program_id_, GL_ACTIVE_ATTRIBUTES, &count);
  printf("Active Attributes: %d\n", count);

  for (i = 0; i < count; i++)
  {
    glGetActiveAttrib(program_id_, (GLuint)i, bufSize, &length, &size, &type, name);

    printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
  }
}

GLint ShaderProgram::lookup_uniform(const char * u, GLenum ty) {
  if (uniform_map_.find(u) == uniform_map_.end()) {
    LOG_ERROR("unknown uniform name '%s'. Shader source out of sync. This message will not print again.", u);
    uniform_map_[u] = std::pair<GLint,GLenum>(-1, -1);
  }

  auto uniform = uniform_map_.at(u);

  if (uniform.second != ty) {
    LOG_ERROR("Type mismatch of uniform '%s' (got %d, expected %d)", u, uniform.second, ty);
    assert(uniform.second == ty);
  }

  return uniform.first;
}

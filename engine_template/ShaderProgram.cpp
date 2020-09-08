#include "ShaderProgram.hpp"

#include <Log.hpp>
#include <sstream>

ShaderProgram::ShaderProgram(std::string name)
  :program_id_(0), linked_(false), name_(name), max_texture_unit_(0)
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

bool ShaderProgram::has_shader(std::shared_ptr<Shader> shader)
{
  for (auto it = shaders_.begin(); it != shaders_.end(); it++) {
    if ((*it) == shader)
      return true;
  }

  return false;
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

  LOG_DEBUG("%s: Linking shader program <- [%s]", name_.c_str(), name_list.c_str());

  GLuint program_id = glCreateProgram();

  if (program_id == 0) {
    LOG_ERROR("%s: Unable to link shader: glCreateProgram failed", name_.c_str());
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
    LOG_ERROR("%s: Failed to link shader program", name_.c_str());

    char * log_buffer = new char[log_length];
    glGetProgramInfoLog(program_id, log_length, NULL, log_buffer);

    LOG_ERROR("~~~~~ SHADER ERROR LOG~~~~~\n%s", log_buffer);
    delete [] log_buffer;

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
  max_texture_unit_ = 0;

  glGetProgramiv(program_id_, GL_ACTIVE_UNIFORMS, &count);

  for (int i = 0; i < count; i++)
  {
      GLint size;
      GLenum type;
      const GLsizei bufSize = 256;
      GLchar nameBuf[bufSize];
      GLsizei length;
      UniformInfo info;

      glGetActiveUniform(program_id_, (GLuint)i, bufSize, &length, &size, &info.type, nameBuf);

      std::string base_name(nameBuf);
      bool is_texture = false;

      // ignore OpenGL/GLSL gl_ prefixed "uniforms"
      if (base_name.length() >= 3 && base_name.substr(0, 3) == "gl_") {
        continue;
      }

      for (int j = 0; j < size; j++) {
        std::string name = base_name;

        if (size > 1) {
          assert(name.substr(name.length()-3) == "[0]");

          std::stringstream ss;
          ss << name.substr(0, name.length()-3) << "[" << j << "]";
          name = ss.str();
        }

        info.location = glGetUniformLocation(program_id_, name.c_str());
        info.texture_unit = 0;

        // other unhandled sampler types will raise errors during set_texture
        if (info.type == GL_SAMPLER_2D) {
          info.texture_unit = max_texture_unit_;
          max_texture_unit_++;
          is_texture = true;
        }

        assert(info.location >= 0);
        assert(uniform_map_.find(name) == uniform_map_.end());

        uniform_map_[name] = info;

        if (is_texture)
          LOG_DEBUG("%s: Shader texture %2d - name=%s ty=%u",
              name_.c_str(), i, name.c_str(), info.type);
        else
          LOG_DEBUG("%s: Shader uniform %2d - name=%s ty=%u",
              name_.c_str(), i, name.c_str(), info.type);
      }
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

#include "uniform_macros.h"

std::string ShaderProgram::format_uniform(L_S1)
{
  return u1;
}

std::string ShaderProgram::format_uniform(L_S1A)
{
  std::stringstream ss;
  ss << u1 << "[" << i1 << "]";
  return ss.str();
}

std::string ShaderProgram::format_uniform(L_S2)
{
  std::stringstream ss;
  ss << u1 << "[" << i1 << "]." << u2;
  return ss.str();
}

std::string ShaderProgram::format_uniform(L_S2A)
{
  std::stringstream ss;
  ss << u1 << "[" << i1 << "]." << u2 << "[" << i2 << "]";
  return ss.str();
}

#include "uniform_macros_undef.h"

GLint ShaderProgram::lookup_uniform(const char * u, GLenum ty) {
  if (uniform_map_.find(u) == uniform_map_.end()) {
    LOG_ERROR("%s: Unknown uniform name '%s'. Shader out of sync. This message will not print again.",
        name_.c_str(), u);
    uniform_map_[u] = {-1, 0, -1};
  }

  auto uniform = uniform_map_.at(u);

  LOG_FATAL_ASSERT(uniform.type == ty || uniform.location == -1,
      "%s: Type mismatch of uniform '%s' (got %d, expected %d)",
      name_.c_str(), u, uniform.type, ty);

  GLint id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
  LOG_FATAL_ASSERT(id == program_id_, "%s: another shader program already in use on uniform", name_.c_str());

  return uniform.location;
}

ShaderProgram::UniformInfo ShaderProgram::lookup_uniform(const char * u, const std::shared_ptr<Texture> & texture)
{
  if (uniform_map_.find(u) == uniform_map_.end()) {
    LOG_ERROR("%s: Unknown uniform name '%s'. Shader out of sync. This message will not print again.",
        name_.c_str(), u);
    uniform_map_[u] = {-1, 0, -1};
  }

  GLenum texture_target = texture->get_texture_target();
  GLenum ty = 0;

  LOG_FATAL_ASSERT(texture_target == GL_TEXTURE_2D, "%s: Unsupported texture type %d for uniform '%s'",
      name_.c_str(), texture_target, u);

  ty = GL_SAMPLER_2D;

  auto uniform = uniform_map_.at(u);
  LOG_FATAL_ASSERT(uniform.type == ty || uniform.location == -1,
      "%s: Type mismatch of uniform '%s' (got %d, expected %d)",
      name_.c_str(), u, uniform.type, ty);

  GLint id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
  LOG_FATAL_ASSERT(id == program_id_, "%s: another shader program already in use on uniform", name_.c_str());

  return uniform;
}

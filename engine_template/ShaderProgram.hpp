#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include <memory>
#include <vector>
#include <map>

#include <Renderer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Log.hpp>

#include <Shader.hpp>
#include <Texture.hpp>

#include "uniform_macros.h"

class ShaderProgram {
  public:
    ShaderProgram(std::string name);
    ~ShaderProgram();

    std::string get_name() { return name_; }
    void add_shader(std::shared_ptr<Shader> shader);
    bool has_shader(std::shared_ptr<Shader> shader);
    bool link();

    inline void use()
    {
      GLint id;
      glGetIntegerv(GL_CURRENT_PROGRAM, &id);

      LOG_FATAL_ASSERT(linked_, "%s: shader program not linked on use", name_.c_str());
      LOG_FATAL_ASSERT(id == 0, "%s: shader program already in use", name_.c_str());

      glUseProgram(program_id_);
    }

    // TODO: should we unbind textures here? how to track which are bound?
    inline void unuse() {
      GLint id;
      glGetIntegerv(GL_CURRENT_PROGRAM, &id);

      LOG_FATAL_ASSERT(linked_, "%s: shader program not linked on unuse", name_.c_str());
      LOG_FATAL_ASSERT(id == program_id_, "%s: another shader program already in use", name_.c_str());

      glUseProgram(0);
    }

    struct UniformInfo {
      GLint location;
      GLenum type;
      GLint texture_unit;
    };

    SET_ARITY(SET_UNIFORM, GLfloat, GL_FLOAT, glUniform1f)
    SET_ARITY(SET_UNIFORM_GLM, glm::vec2, GL_FLOAT_VEC2, glUniform2fv)
    SET_ARITY(SET_UNIFORM_GLM, glm::vec3, GL_FLOAT_VEC3, glUniform3fv)
    SET_ARITY(SET_UNIFORM_GLM, glm::vec4, GL_FLOAT_VEC4, glUniform4fv)

    SET_ARITY(SET_UNIFORM, GLint, GL_INT, glUniform1i)
    SET_ARITY(SET_UNIFORM_GLM, glm::ivec2, GL_INT_VEC2, glUniform2iv)
    SET_ARITY(SET_UNIFORM_GLM, glm::ivec3, GL_INT_VEC3, glUniform3iv)
    SET_ARITY(SET_UNIFORM_GLM, glm::ivec4, GL_INT_VEC4, glUniform4iv)

    // TODO: types seem to be missing...
    /*SET_UNIFORM(GLuint, GL_UNSIGNED_INT, glUniform1uiv, &v)
    SET_UNIFORM_GLM(glm::uvec2, GL_UNSIGNED_INT_VEC2, glUniform2uiv)
    SET_UNIFORM_GLM(glm::uvec3, GL_UNSIGNED_INT_VEC3, glUniform3uiv)
    SET_UNIFORM_GLM(glm::uvec4, GL_UNSIGNED_INT_VEC2, glUniform4uiv)*/

    SET_ARITY(SET_UNIFORM, bool, GL_BOOL, glUniform1i)

    SET_ARITY(SET_UNIFORM_MAT, glm::mat2, GL_FLOAT_MAT2, glUniformMatrix2fv)
    SET_ARITY(SET_UNIFORM_MAT, glm::mat3, GL_FLOAT_MAT3, glUniformMatrix3fv)
    SET_ARITY(SET_UNIFORM_MAT, glm::mat4, GL_FLOAT_MAT4, glUniformMatrix4fv)

    SET_UNIFORM_TEX(1)
    SET_UNIFORM_TEX(1A)
    SET_UNIFORM_TEX(2)
    SET_UNIFORM_TEX(2A)

  private:
    std::string format_uniform(L_S1);
    std::string format_uniform(L_S1A);
    std::string format_uniform(L_S2);
    std::string format_uniform(L_S2A);

    GLint lookup_uniform(const char * u, GLenum ty);
    UniformInfo lookup_uniform(const char * u, const std::shared_ptr<Texture> & texture);

    void refresh_uniforms();
    void refresh_attributes();
    void release();

    std::string name_;
    std::vector<std::shared_ptr<Shader>> shaders_;
    GLuint program_id_;

    std::map<std::string, UniformInfo> uniform_map_;
    bool linked_, in_use_;
    int max_texture_unit_;
};

#include "uniform_macros_undef.h"

#endif // _SHADER_PROGRAM_H

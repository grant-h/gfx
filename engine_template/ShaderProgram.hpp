#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include <memory>
#include <vector>
#include <map>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.hpp>

#define SET_UNIFORM(ty, e, fn, val) \
    inline void set_uniform(const char * u, const ty & v) { \
      GLint id = lookup_uniform(u, e); \
      if (id >= 0) fn(id, 1, val); \
    }
#define SET_UNIFORM_MAT(ty, e, fn ) \
    inline void set_uniform(const char * u, const ty & v) { \
      GLint id = lookup_uniform(u, e); \
      if (id >= 0) fn(id, 1, GL_FALSE, glm::value_ptr(v)); \
    }
#define SET_UNIFORM_GLM(ty, e, fn) SET_UNIFORM(ty, e, fn, glm::value_ptr(v))

class ShaderProgram {
  public:
    ShaderProgram();
    ~ShaderProgram();

    void add_shader(std::shared_ptr<Shader> shader);
    bool link();
    GLuint get_program_id();

    SET_UNIFORM(GLfloat, GL_FLOAT, glUniform1fv, &v)
    SET_UNIFORM_GLM(glm::vec2, GL_FLOAT_VEC2, glUniform2fv)
    SET_UNIFORM_GLM(glm::vec3, GL_FLOAT_VEC3, glUniform3fv)
    SET_UNIFORM_GLM(glm::vec4, GL_FLOAT_VEC4, glUniform4fv)

    SET_UNIFORM(GLint, GL_INT, glUniform1iv, &v)
    SET_UNIFORM_GLM(glm::ivec2, GL_INT_VEC2, glUniform2iv)
    SET_UNIFORM_GLM(glm::ivec3, GL_INT_VEC3, glUniform3iv)
    SET_UNIFORM_GLM(glm::ivec4, GL_INT_VEC4, glUniform4iv)

    SET_UNIFORM_MAT(glm::mat2, GL_FLOAT_MAT2, glUniformMatrix2fv)
    SET_UNIFORM_MAT(glm::mat3, GL_FLOAT_MAT3, glUniformMatrix3fv)
    SET_UNIFORM_MAT(glm::mat4, GL_FLOAT_MAT4, glUniformMatrix4fv)

    // TODO: types seem to be missing...
    /*SET_UNIFORM(GLuint, GL_UNSIGNED_INT, glUniform1uiv, &v)
    SET_UNIFORM_GLM(glm::uvec2, GL_UNSIGNED_INT_VEC2, glUniform2uiv)
    SET_UNIFORM_GLM(glm::uvec3, GL_UNSIGNED_INT_VEC3, glUniform3uiv)
    SET_UNIFORM_GLM(glm::uvec4, GL_UNSIGNED_INT_VEC2, glUniform4uiv)*/
  private:
    GLint lookup_uniform(const char * u, GLenum ty);
    void refresh_uniforms();
    void refresh_attributes();
    void release();

    std::vector<std::shared_ptr<Shader>> shaders_;
    GLuint program_id_;
    std::map<std::string, std::pair<GLint,GLenum>> uniform_map_;
    bool linked_;
};

#undef SET_UNIFORM_MAT
#undef SET_UNIFORM_GLM
#undef SET_UNIFORM

#endif // _SHADER_PROGRAM_H

#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include <memory>
#include <vector>
#include <map>

#include <Renderer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.hpp>
#include <Texture.hpp>

#define SET_UNIFORM(ty, e, fn) \
    inline void set_uniform(const char * u, const ty v) { \
      GLint id = lookup_uniform(u, e); \
      if (id >= 0) fn(id, v); \
    }
#define SET_UNIFORM_MAT(ty, e, fn ) \
    inline void set_uniform(const char * u, const ty & v) { \
      GLint id = lookup_uniform(u, e); \
      if (id >= 0) fn(id, 1, GL_FALSE, glm::value_ptr(v)); \
    }

#define SET_UNIFORM_GLM(ty, e, fn) \
    inline void set_uniform(const char * u, const ty & v) { \
      GLint id = lookup_uniform(u, e); \
      if (id >= 0) fn(id, 1, glm::value_ptr(v)); \
    }

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
      if (!linked_)
        return;

      glUseProgram(program_id_);
    }

    // TODO: should we unbind textures here? how to track which are bound?
    inline void unuse() { glUseProgram(0); }

    SET_UNIFORM(GLfloat, GL_FLOAT, glUniform1f)
    SET_UNIFORM_GLM(glm::vec2, GL_FLOAT_VEC2, glUniform2fv)
    SET_UNIFORM_GLM(glm::vec3, GL_FLOAT_VEC3, glUniform3fv)
    SET_UNIFORM_GLM(glm::vec4, GL_FLOAT_VEC4, glUniform4fv)

    SET_UNIFORM(GLint, GL_INT, glUniform1i)
    SET_UNIFORM_GLM(glm::ivec2, GL_INT_VEC2, glUniform2iv)
    SET_UNIFORM_GLM(glm::ivec3, GL_INT_VEC3, glUniform3iv)
    SET_UNIFORM_GLM(glm::ivec4, GL_INT_VEC4, glUniform4iv)

    SET_UNIFORM(bool, GL_BOOL, glUniform1i)

    SET_UNIFORM_MAT(glm::mat2, GL_FLOAT_MAT2, glUniformMatrix2fv)
    SET_UNIFORM_MAT(glm::mat3, GL_FLOAT_MAT3, glUniformMatrix3fv)
    SET_UNIFORM_MAT(glm::mat4, GL_FLOAT_MAT4, glUniformMatrix4fv)

    struct UniformInfo {
      GLint location;
      GLenum type;
      GLint texture_unit;
    };

    inline void set_texture(const char * u, const std::shared_ptr<Texture> & texture) {
      UniformInfo info = lookup_uniform_texture(u, texture);

      if (info.location >= 0) {
        glUniform1i(info.location, info.texture_unit);
        glActiveTexture(GL_TEXTURE0 + info.texture_unit);
        texture->use();
      }
    }

    // TODO: types seem to be missing...
    /*SET_UNIFORM(GLuint, GL_UNSIGNED_INT, glUniform1uiv, &v)
    SET_UNIFORM_GLM(glm::uvec2, GL_UNSIGNED_INT_VEC2, glUniform2uiv)
    SET_UNIFORM_GLM(glm::uvec3, GL_UNSIGNED_INT_VEC3, glUniform3uiv)
    SET_UNIFORM_GLM(glm::uvec4, GL_UNSIGNED_INT_VEC2, glUniform4uiv)*/
  private:
    GLint lookup_uniform(const char * u, GLenum ty);
    UniformInfo lookup_uniform_texture(const char * u, const std::shared_ptr<Texture> & texture);

    void refresh_uniforms();
    void refresh_attributes();
    void release();

    std::string name_;
    std::vector<std::shared_ptr<Shader>> shaders_;
    GLuint program_id_;

    std::map<std::string, UniformInfo> uniform_map_;
    bool linked_;
    int max_texture_unit_;
};

#undef SET_UNIFORM_MAT
#undef SET_UNIFORM_GLM
#undef SET_UNIFORM

#endif // _SHADER_PROGRAM_H

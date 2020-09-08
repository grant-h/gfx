#define L_S1   const char * u1
#define L_S1A  L_S1, unsigned int i1
#define L_S2   L_S1A, const char * u2
#define L_S2A  L_S2, unsigned int i2
#define L_C1  u1
#define L_C1A L_C1, i1
#define L_C2  L_C1A, u2
#define L_C2A L_C2, i2
#define L_NORMAL(va) va, GLenum ty
#define L_TEXTURE(va) va, const std::shared<Texture> & texture

#define SET_ARITY(macro, ty, e, fn) \
    macro(1, ty, e, fn) \
    macro(1A, ty, e, fn) \
    macro(2, ty, e, fn) \
    macro(2A, ty, e, fn)

#define SET_UNIFORM(arity, ty, e, fn) \
    inline void set_uniform(L_S ## arity, const ty v) { \
      GLint id = lookup_uniform(format_uniform(L_C ## arity).c_str(), e); \
      if (id >= 0) fn(id, v); \
    }

#define SET_UNIFORM_MAT(arity, ty, e, fn) \
    inline void set_uniform(L_S ## arity, const ty & v) { \
      GLint id = lookup_uniform(format_uniform(L_C ## arity).c_str(), e); \
      if (id >= 0) fn(id, 1, GL_FALSE, glm::value_ptr(v)); \
    }

#define SET_UNIFORM_GLM(arity, ty, e, fn) \
    inline void set_uniform(L_S ## arity, const ty & v) { \
      GLint id = lookup_uniform(format_uniform(L_C ## arity).c_str(), e); \
      if (id >= 0) fn(id, 1, glm::value_ptr(v)); \
    }

#define SET_UNIFORM_TEX(arity) \
    inline void set_uniform(L_S ## arity, const std::shared_ptr<Texture> & texture) { \
      UniformInfo info = lookup_uniform(format_uniform(L_C ## arity).c_str(), texture); \
      if (info.location >= 0) { \
        glUniform1i(info.location, info.texture_unit); \
        glActiveTexture(GL_TEXTURE0 + info.texture_unit); \
        texture->use(); \
      } \
    }

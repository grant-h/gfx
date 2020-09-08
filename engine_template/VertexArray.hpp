#ifndef _VERTEX_ARRAY_H
#define _VERTEX_ARRAY_H

#include <Renderer.hpp>

#include <vector>
#include <memory>
#include <string>

struct Vertex {
  union {
    struct { float x, y, z, w; };
    float position[4];
  };
};

struct VertexC {
  union {
    struct { float x, y, z, w; };
    float position[4];
  };
  union {
    struct { float r, g, b, a; };
    float color[4];
  };
};

struct VertexCN {
  union {
    struct { float x, y, z, w; };
    float position[4];
  };
  union {
    struct { float r, g, b, a; };
    float color[4];
  };
  union {
    struct { float nx, ny, nz, nw; };
    float normal[4];
  };
};

struct VertexCNT {
  union {
    struct { float x, y, z, w; };
    float position[4];
  };
  union {
    struct { float r, g, b, a; };
    float color[4];
  };
  union {
    struct { float nx, ny, nz, nw; };
    float normal[4];
  };
  union {
    struct { float u, v; };
    float texture[2];
  };
};

struct ShaderAttribute {
  GLint index;
  std::string name;

  GLenum type;
  GLint nelem;
  GLsizei stride;
  GLsizei offset;
};

class VertexArray {
  public:
    VertexArray(GLenum draw_type);
    ~VertexArray();

    bool create(std::vector<Vertex> & verts);
    bool create(std::vector<Vertex> & verts, std::vector<uint32_t> & indicies);
    bool create(std::vector<VertexC> & verts);
    bool create(std::vector<VertexCNT> & verts);
    bool create(std::vector<VertexCNT> & verts, std::vector<uint32_t> & indicies);

    void draw();
    void draw(GLenum draw_type);
  private:
    bool create_internal(void * verticies, GLsizei vertex_stride, void * indicies, GLsizei index_stride);
    void add_attribute(const char * name, GLenum type, GLint nelem, GLsizei stride, GLsizei offset);

    void activate();
    void deactivate();
    void release();

    VertexArray(const VertexArray&);
    VertexArray& operator=(const VertexArray&);

    size_t vertex_count_;
    size_t index_count_;

    GLenum draw_type_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_id_;
    GLuint index_buffer_id_;
    std::vector<ShaderAttribute> attributes_;
};

/*class ScopedVertexArray {
  public:
    ScopedVertexArray(VertexArray * array) : array_(array)
    {
      array_->activate();
    }

    ~ScopedVertexArray()
    {
      array_->deactivate();
    }
  private:
    VertexArray * array_;
};*/

#endif // _VERTEX_ARRAY_H

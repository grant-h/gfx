#ifndef _VERTEX_ARRAY_H
#define _VERTEX_ARRAY_H

#include <GL/glew.h>

#include <vector>
#include <memory>

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

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    bool create(std::vector<Vertex> & verts);
    bool create(std::vector<VertexC> & verts);
    bool create(std::vector<VertexCNT> & verts);
    void activate();
    void deactivate();
    size_t size() { return size_; }
  private:
    void release();
    VertexArray(const VertexArray&);
    VertexArray& operator=(const VertexArray&);

    size_t size_;
    GLuint vertex_array_id_;
    GLuint vertex_buffer_id_;
    GLuint index_buffer_id_;
};

class ScopedVertexArray {
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
};

#endif // _VERTEX_ARRAY_H

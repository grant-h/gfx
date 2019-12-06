#ifndef _VERTEX_ARRAY_H
#define _VERTEX_ARRAY_H

#include <GL/glew.h>

#include <vector>
#include <memory>

struct Vertex {
  float x, y, z, w;
};

struct VertexC {
  float x, y, z, w;
  float r, g, b, a;
};

struct VertexCN {
  float x, y, z, w;
  float r, g, b, a;
  float nx, ny, nz, nw;
};

struct VertexCNT {
  float x, y, z, w;
  float r, g, b, a;
  float nx, ny, nz, nw;
  float u, v;
};

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    bool create(std::vector<Vertex> & verts);
    void activate();
    void deactivate();
  private:
    void release();
    VertexArray(const VertexArray&);
    VertexArray& operator=(const VertexArray&);

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

#include "VertexArray.hpp"

#include <Log.hpp>

#include <cstddef>

VertexArray::VertexArray()
  :vertex_array_id_(GL_INVALID_VALUE), vertex_buffer_id_(GL_INVALID_VALUE), index_buffer_id_(GL_INVALID_VALUE),
  size_(0)
{
}

VertexArray::~VertexArray()
{
  release();
}

void VertexArray::release()
{
  if (index_buffer_id_ != GL_INVALID_VALUE)
    glDeleteBuffers(1, &index_buffer_id_);

  if (vertex_buffer_id_ != GL_INVALID_VALUE)
    glDeleteBuffers(1, &vertex_buffer_id_);

  if (vertex_array_id_ != GL_INVALID_VALUE)
    glDeleteVertexArrays(1, &vertex_array_id_);

  vertex_array_id_ = GL_INVALID_VALUE;
  vertex_buffer_id_ = GL_INVALID_VALUE;
  index_buffer_id_ = GL_INVALID_VALUE;
  size_ = 0;
}

bool VertexArray::create(std::vector<Vertex> & verts)
{
  release();

  GLenum ErrorCheckValue = glGetError();

  // Create Vertex Array Object
  glGenVertexArrays(1, &vertex_array_id_);
  glBindVertexArray(vertex_array_id_);

  // Create Buffer for vertex data
  glGenBuffers(1, &vertex_buffer_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*verts.size(), verts.data(), GL_STATIC_DRAW);

  size_ = verts.size();

  // Assign vertex attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<void*>(offsetof(Vertex, position)));

  glEnableVertexAttribArray(0); // position

  // Disable our Vertex Buffer Object 
  glBindVertexArray(0);

  ErrorCheckValue = glGetError();

  if (ErrorCheckValue != GL_NO_ERROR) {
    release();
    return false;
  }

  size_ = verts.size();

  LOG_DEBUG("VertexArray %p: createP %d", this, verts.size());

  return true;
}

bool VertexArray::create(std::vector<VertexC> & verts)
{
  release();

  GLenum ErrorCheckValue = glGetError();

  // Create Vertex Array Object
  glGenVertexArrays(1, &vertex_array_id_);
  glBindVertexArray(vertex_array_id_);

  // Create Buffer for vertex data
  glGenBuffers(1, &vertex_buffer_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexC)*verts.size(), verts.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); // position
  glEnableVertexAttribArray(1); // color

  // Assign vertex attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
      sizeof(VertexC), reinterpret_cast<void*>(offsetof(VertexC, x)));
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
      sizeof(VertexC), reinterpret_cast<void*>(offsetof(VertexC, r)));

  // Disable our Vertex Buffer Object 
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  ErrorCheckValue = glGetError();

  if (ErrorCheckValue != GL_NO_ERROR) {
    release();
    return false;
  }

  size_ = verts.size();

  LOG_DEBUG("VertexArray (%d, %d): createPC %d", vertex_array_id_,
      vertex_buffer_id_, verts.size());

  return true;
}

bool VertexArray::create(std::vector<VertexCNT> & verts)
{
  release();

  GLenum ErrorCheckValue = glGetError();

  // Create Vertex Array Object
  glGenVertexArrays(1, &vertex_array_id_);
  glBindVertexArray(vertex_array_id_);

  // Create Buffer for vertex data
  glGenBuffers(1, &vertex_buffer_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexCNT)*verts.size(), verts.data(), GL_STATIC_DRAW);

  // Assign vertex attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
      sizeof(VertexCNT), reinterpret_cast<void*>(offsetof(VertexCNT, position)));
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
      sizeof(VertexCNT), reinterpret_cast<void*>(offsetof(VertexCNT, color)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 
      sizeof(VertexCNT), reinterpret_cast<void*>(offsetof(VertexCNT, normal)));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 
      sizeof(VertexCNT), reinterpret_cast<void*>(offsetof(VertexCNT, texture)));

  /*for (int i = 0; i < verts.size(); i++) {
    VertexCNT * v1 = (verts.data() + i);
    printf("%d: xyzw %.2f %.2f %.2f %.2f\n", i, v1[0].x, v1[0].y, v1[0].z, v1[0].w);
    //printf("%d: posi %.2f %.2f %.2f %.2f\n", i, v1[0].position[0], v1[0].position[1],v1[0].position[2],v1[0].position[3]);
    printf("%d: rgba %.2f %.2f %.2f %.2f\n\n", i, v1[0].r, v1[0].g, v1[0].b, v1[0].a);
  }*/

  glEnableVertexAttribArray(0); // position
  glEnableVertexAttribArray(1); // color
  glEnableVertexAttribArray(2); // normal
  glEnableVertexAttribArray(3); // texture

  // Disable our Vertex Buffer Object 
  glBindVertexArray(0);

  ErrorCheckValue = glGetError();

  if (ErrorCheckValue != GL_NO_ERROR) {
    release();
    return false;
  }

  size_ = verts.size();

  return true;
}

void VertexArray::activate()
{
  assert(vertex_array_id_ != GL_INVALID_VALUE);
  glBindVertexArray(vertex_array_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
}

void VertexArray::deactivate()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

#include "VertexArray.hpp"

#include <Log.hpp>

#include <cstddef>

VertexArray::VertexArray()
  :vertex_array_id_(GL_INVALID_VALUE), vertex_buffer_id_(GL_INVALID_VALUE), index_buffer_id_(GL_INVALID_VALUE)
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

  // Assign vertex attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float), reinterpret_cast<void*>(offsetof(Vertex, x)));

  glEnableVertexAttribArray(0); // position

  // Disable our Vertex Buffer Object 
  glBindVertexArray(0);

  ErrorCheckValue = glGetError();

  if (ErrorCheckValue != GL_NO_ERROR) {
    release();
    return false;
  }

  LOG_DEBUG("VertexArray %p: createP %d", this, verts.size());

  return true;
}

void VertexArray::activate()
{
  assert(vertex_array_id_ != GL_INVALID_VALUE);
  glBindVertexArray(vertex_array_id_);
}

void VertexArray::deactivate()
{
  glBindVertexArray(0);
}

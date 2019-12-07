#include "VertexArray.hpp"

#include <Log.hpp>

#include <cstddef>

VertexArray::VertexArray(GLenum draw_type)
  :vertex_array_id_(GL_INVALID_VALUE), vertex_buffer_id_(GL_INVALID_VALUE), index_buffer_id_(GL_INVALID_VALUE),
  vertex_count_(0), index_count_(0), draw_type_(draw_type)
{
}

VertexArray::~VertexArray()
{
  release();
}

bool VertexArray::create(std::vector<Vertex> & verts)
{
  attributes_.clear();
  add_attribute("in_position", GL_FLOAT, 4, sizeof(Vertex), offsetof(Vertex, position));

  vertex_count_ = verts.size();
  index_count_ = 0;

  return create_internal(verts.data(), sizeof(Vertex), nullptr, 0);
}

bool VertexArray::create(std::vector<Vertex> & verts, std::vector<uint32_t> & indicies)
{
  attributes_.clear();
  add_attribute("in_position", GL_FLOAT, 4, sizeof(Vertex), offsetof(Vertex, position));

  vertex_count_ = verts.size();
  index_count_ = indicies.size();

  return create_internal(verts.data(), sizeof(Vertex), indicies.data(), sizeof(uint32_t));
}

bool VertexArray::create(std::vector<VertexC> & verts)
{
  attributes_.clear();
  add_attribute("in_position", GL_FLOAT, 4, sizeof(VertexC), offsetof(VertexC, position));
  add_attribute("in_color", GL_FLOAT, 4, sizeof(VertexC), offsetof(VertexC, color));

  vertex_count_ = verts.size();
  index_count_ = 0;

  return create_internal(verts.data(), sizeof(VertexC), nullptr, 0);
}

bool VertexArray::create(std::vector<VertexCNT> & verts)
{
  attributes_.clear();
  add_attribute("in_position", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, position));
  add_attribute("in_color", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, color));
  add_attribute("in_normal", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, normal));
  add_attribute("in_texcoord", GL_FLOAT, 2, sizeof(VertexCNT), offsetof(VertexCNT, texture));

  vertex_count_ = verts.size();
  index_count_ = 0;

  return create_internal(verts.data(), sizeof(VertexCNT), nullptr, 0);
}

bool VertexArray::create(std::vector<VertexCNT> & verts, std::vector<uint32_t> & indicies)
{
  attributes_.clear();
  add_attribute("in_position", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, position));
  add_attribute("in_color", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, color));
  add_attribute("in_normal", GL_FLOAT, 4, sizeof(VertexCNT), offsetof(VertexCNT, normal));
  add_attribute("in_texcoord", GL_FLOAT, 2, sizeof(VertexCNT), offsetof(VertexCNT, texture));

  vertex_count_ = verts.size();
  index_count_ = indicies.size();

  return create_internal(verts.data(), sizeof(VertexCNT), indicies.data(), sizeof(uint32_t));
}

void VertexArray::draw()
{
  activate();

  if (index_count_ > 0) {
    glDrawElements(draw_type_, index_count_, GL_UNSIGNED_INT, (void *)0);
  } else {
    glDrawArrays(draw_type_, 0, vertex_count_);
  }

  deactivate();
}

//////////////////////////


bool VertexArray::create_internal(void * verticies, GLsizei vertex_stride,
    void * indicies, GLsizei index_stride)
{
  GLenum ErrorCheckValue = glGetError();

  // Create Vertex Array Object
  glGenVertexArrays(1, &vertex_array_id_);

  activate();

  // Create Buffer for vertex data
  glGenBuffers(1, &vertex_buffer_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
  glBufferData(GL_ARRAY_BUFFER, vertex_stride*vertex_count_, verticies, GL_STATIC_DRAW);

  if (indicies) {
    glGenBuffers(1, &index_buffer_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_stride*index_count_, indicies, GL_STATIC_DRAW);
  }

  for (int i = 0; i < attributes_.size(); i++) {
    ShaderAttribute & att = attributes_.at(i);
    // Assign vertex attributes
    glVertexAttribPointer(att.index, att.nelem, att.type, GL_FALSE, att.stride,
        reinterpret_cast<void*>(att.offset));
    glEnableVertexAttribArray(att.index);
  }

  deactivate();

  ErrorCheckValue = glGetError();

  if (ErrorCheckValue != GL_NO_ERROR) {
    release();
    return false;
  } else {
    std::string attribute_str;
    for (int i = 0; i < attributes_.size(); i++)
      attribute_str += attributes_.at(i).name + (i+1==attributes_.size() ? "" : ",");

    if (index_count_)
      LOG_DEBUG("VertexArray: createIndex[%s] %d, %d", attribute_str.c_str(),
          vertex_count_, index_count_);
    else
      LOG_DEBUG("VertexArray: create[%s] %d", attribute_str.c_str(),
          vertex_count_);

    return true;
  }
}

void VertexArray::add_attribute(const char * name, GLenum type, GLint nelem, GLsizei stride, GLsizei offset)
{
  ShaderAttribute att;
  att.index = attributes_.size();
  att.name = name;
  att.type = type;
  att.nelem = nelem;
  att.stride = stride;
  att.offset = offset;

  attributes_.push_back(att);
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

  vertex_count_ = 0;
  index_count_ = 0;
}


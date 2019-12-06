#include "PointObject.hpp"

#include <sstream>

#include <Log.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ShaderProgram.hpp>

PointObject::PointObject(const char * name)
  :SceneObject(name)
{
}

PointObject::PointObject(const char * name, std::shared_ptr<SceneObject> parent)
  :SceneObject(name, parent)
{
}

PointObject::~PointObject()
{
  LOG_TRACE_FUNC("delete");
}

bool PointObject::init()
{
  std::vector<VertexC> vec;
  VertexC v = {0.0, 0.0, 0.0, 1.0, color_.r, color_.g, color_.b, 1.0};
  vec.push_back(v);
  if (!vao_.create(vec))
    return false;

  GLuint program_id = shader_->get_program_id();

  if (program_id == 0) {
    return false;
  }

  programM_ = glGetUniformLocation(program_id, "M");
  programV_ = glGetUniformLocation(program_id, "V");
  programP_ = glGetUniformLocation(program_id, "P");

  if (programM_ < 0 || programV_ < 0 || programP_ < 0) {
    return false;
  }

  return true;
}

void PointObject::tick()
{
}

void PointObject::draw(std::shared_ptr<CameraObject> camera)
{
  ScopedVertexArray sva(&vao_);

  glUseProgram(shader_->get_program_id());
  glEnable(GL_PROGRAM_POINT_SIZE);

  glUniformMatrix4fv(programM_, 1, GL_FALSE, &get_model_matrix()[0][0]);
  glUniformMatrix4fv(programV_, 1, GL_FALSE, &camera->get_view_matrix()[0][0]);
  glUniformMatrix4fv(programP_, 1, GL_FALSE, &camera->get_projection_matrix()[0][0]);

  glDrawArrays(GL_POINTS, 0, 1);

  glDisable(GL_PROGRAM_POINT_SIZE);
  glUseProgram(0);
}

void PointObject::set_color(float x, float y, float z)
{
  color_ = glm::vec3(x, y, z);
}

void PointObject::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

#include "PointObject.hpp"

#include <sstream>

#include <Log.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ShaderProgram.hpp>
#include <ResourceManager.hpp>

PointObject::PointObject(const char * name)
  :SceneObject(name)
{
}

PointObject::~PointObject()
{
}

bool PointObject::init()
{
  std::vector<VertexC> vec {{.position={0.0, 0.0, 0.0, 1.0,}, .color={color_.r, color_.g, color_.b, 1.0}}};

  vao_ = std::unique_ptr<VertexArray>(new VertexArray(GL_POINTS));
  shader_ = ResourceManager::instance()->get_shader_program("Point");

  if (!vao_->create(vec))
    return false;

  return true;
}

void PointObject::tick()
{
}

void PointObject::draw(std::shared_ptr<CameraObject> camera)
{
  shader_->use();

  glEnable(GL_PROGRAM_POINT_SIZE);

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("camera", camera->get_eye());

  vao_->draw();

  glDisable(GL_PROGRAM_POINT_SIZE);

  shader_->unuse();
}

void PointObject::set_color(float x, float y, float z)
{
  color_ = glm::vec3(x, y, z);
}

void PointObject::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

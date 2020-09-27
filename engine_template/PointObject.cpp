#include "PointObject.hpp"

#include <sstream>

#include <Log.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <SceneRenderer.hpp>
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

  vao_ = std::shared_ptr<VertexArray>(new VertexArray(GL_POINTS));
  shader_ = ResourceManager::instance()->get_shader_program("Point");

  if (!vao_->create(vec))
    return false;

  return true;
}

void PointObject::tick()
{
}

void PointObject::draw(SceneRenderer * render)
{
  render->draw_point(this, vao_, shader_);
}

void PointObject::set_color(float x, float y, float z)
{
  color_ = glm::vec3(x, y, z);
}

void PointObject::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

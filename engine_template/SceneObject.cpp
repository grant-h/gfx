#include "SceneObject.hpp"

#include <sstream>

#include <Log.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

SceneObject::SceneObject(const char * name)
  :scale_(1.0f), object_name_(name)
{
}

SceneObject::~SceneObject()
{
}

glm::vec3 SceneObject::position()
{
  return position_;
}

void SceneObject::position(glm::vec3 & pos)
{
  position_ = pos;
}

void SceneObject::position(float x, float y, float z)
{
  position_ = glm::vec3(x, y, z);
}

glm::mat4 SceneObject::get_model_matrix()
{
  using namespace glm;
  glm::mat4 m = glm::mat4(1.0);

  glm::quat q = glm::quat(rotation_);
  glm::mat4 rot = mat4_cast(q);
  glm::mat4 trans = glm::translate(m, position_);
  glm::mat4 sca = glm::scale(m, glm::vec3(scale_));

  if (auto parent = parent_object_.lock()) {
    m = parent->get_model_matrix() * trans * rot * sca * m;
  } else {
    m = trans * rot * sca * m;
  }

  return m;

  // TODO: parent
  /*if(m_parent) {
    return m_parent->getModelMatrix() * m;
  } else {
    return m;
  }*/
}

void SceneObject::add_child_object(std::shared_ptr<SceneObject> child) {
  child->parent_object_ = shared_from_this();
  child_objects_.push_back(child);
}

std::string SceneObject::to_string()
{
  std::ostringstream ss;

  ss << "Obj[" << object_name_ << "]<" <<
    position_.x << ", " <<
    position_.y << ", " <<
    position_.z << ">";

  return ss.str();
}


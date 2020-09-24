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
  // TODO: cache matrix result
  using namespace glm;
  mat4 m = mat4(1.0);

  quat q = quat(rotation_);
  mat4 rot = mat4_cast(q);
  mat4 trans = translate(m, position_);
  mat4 sca = scale(m, scale_);

  if (auto parent = parent_object_.lock()) {
    m = parent->get_model_matrix() * trans * rot * sca * m;
  } else {
    m = trans * rot * sca * m;
  }

  return m;
}

glm::mat4 SceneObject::get_normal_matrix()
{
  return glm::transpose(glm::inverse(get_model_matrix()));
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


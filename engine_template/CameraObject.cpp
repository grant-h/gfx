#include <CameraObject.hpp>

#define PI 3.1415926535

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

CameraObject::CameraObject(const char * name)
  :SceneObject(name), aspect_ratio_(1.0), near_(0.1), far_(500.0), fov_(90.0f), yaw_(0.0), pitch_(0.0)
{
  calculate_view();
}

CameraObject::CameraObject(const char * name, std::shared_ptr<SceneObject> parent)
  :SceneObject(name, std::move(parent))
{
  calculate_view();
}

CameraObject::~CameraObject()
{
}

void CameraObject::position(glm::vec3 & pos)
{
  SceneObject::position(pos);
  calculate_view();
}

void CameraObject::position(float x, float y, float z)
{
  SceneObject::position(x, y, z);
  calculate_view();
}

glm::mat4 CameraObject::get_view_matrix()
{
  calculate_view();
  return view_;
}

glm::mat4 CameraObject::get_projection_matrix()
{
  return projection_;
}

void CameraObject::set_fov(float fov)
{
  fov_ = fov;
  calculate_view();
}

void CameraObject::set_near_clip(float near)
{
  near_ = near;
  calculate_view();
}

void CameraObject::set_far_clip(float far)
{
  far_ = far;
  calculate_view();
}

void CameraObject::set_aspect_ratio(float aspect)
{
  aspect_ratio_ = aspect;
  calculate_view();
}

#include <Renderer.hpp>

void CameraObject::calculate_view()
{
  glm::mat4 m = glm::mat4(1.0);

  // An orbiting camera
  glm::quat q = glm::quat(glm::vec3(-pitch_, yaw_, 0.0));
  glm::mat4 rot = mat4_cast(q);
  glm::mat4 trans = glm::translate(m, glm::vec3(0.0, 0.0, 1.0));

  glm::vec3 center = SceneObject::position();
  glm::vec4 pos = glm::translate(m, center) * rot * trans * glm::vec4(glm::vec3(0.0), 1.0);

  camera_eye_.x = pos.x;
  camera_eye_.y = pos.y;
  camera_eye_.z = pos.z;

  glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

  view_ = glm::lookAt(camera_eye_, center, up);
  projection_ = glm::perspective(glm::radians(fov_), aspect_ratio_, near_, far_);
}


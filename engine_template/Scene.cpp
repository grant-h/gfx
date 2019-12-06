#include "Scene.hpp"

#include <Log.hpp>
#include <Window.hpp>
#include <CameraObject.hpp>

Scene::Scene(const char * name)
  :scene_name_(name)
{
}

Scene::~Scene()
{
  LOG_TRACE_FUNC("delete");
}

void Scene::add_object(SceneObject * obj)
{
  objects_.push_back(std::shared_ptr<SceneObject>(obj));
}

void Scene::add_object(std::shared_ptr<SceneObject> obj)
{
  objects_.push_back(std::move(obj));
}

void Scene::set_camera(std::shared_ptr<CameraObject> camera)
{
  active_camera_ = camera;
}

void Scene::print_objects()
{
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    LOG_INFO("%s", (*it)->to_string().c_str());
  }
}

#include <GLFW/glfw3.h>
void Scene::tick()
{
  glm::vec3 mod(0.0f, 0.0f, (sin(glfwGetTime()) + 1.0f)*4.0f - 2.0f);
  //float fov = 90.0f + sin(glfwGetTime())*40.0f;
  active_camera_->position(mod);
  //active_camera_->set_fov(fov);

  objects_.at(0)->position(mod);

  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->tick();
  }
}

void Scene::draw()
{
  if (active_camera_ == nullptr)
    return;

  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->draw(active_camera_);
  }
}

void Scene::resize(Window * window)
{
  // TODO: resize all cameras
  if (active_camera_) {
    LOG_DEBUG("Resize camera %dx%d", window->get_width(),window->get_height());
    active_camera_->set_aspect_ratio((float)window->get_width()/window->get_height());
  }
}
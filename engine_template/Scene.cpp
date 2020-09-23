#include "Scene.hpp"

#include <Log.hpp>
#include <CameraObject.hpp>
#include <Renderer.hpp>

Scene::Scene(const char * name)
  :scene_name_(name)
{
}

Scene::~Scene()
{
}

void Scene::add_object(SceneObject * obj)
{
  objects_.push_back(std::shared_ptr<SceneObject>(obj));
}

void Scene::add_object(std::shared_ptr<SceneObject> obj)
{
  objects_.push_back(std::move(obj));
}

void Scene::set_camera(std::shared_ptr<CameraController> camera)
{
  active_camera_ = camera;
}

void Scene::print_objects()
{
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    LOG_INFO("%s", (*it)->to_string().c_str());
  }
}

void Scene::tick()
{
  glm::vec3 mod(0.0f, 0.0f, (sin(glfwGetTime()) + 1.0f)*4.0f - 2.0f);
  //float fov = 90.0f + sin(glfwGetTime())*40.0f;
  //active_camera_->set_lat_lon(glfwGetTime()*0.5, 0.0);
  //active_camera_->position(mod);
  //active_camera_->set_fov(fov);

  /*glm::vec3 mod2(0.0f, (sin(glfwGetTime()) + 1.0f)*4.0f - 2.0f, 0.0f);
  objects_.at(1)->position(mod2);*/

  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->tick();
  }
}

void Scene::debug_select_camera()
{
  static int e = 1;
  int i = 0;

  DGUI_BEGIN;

    ImGui::Begin("Camera Controller");
    ImGui::RadioButton("No camera", &e, 0);

    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        auto camera = std::dynamic_pointer_cast<CameraController>(*it);

        if (!camera)
          continue;

        if (e == i+1 && active_camera_ != camera)
          active_camera_ = camera;
        else if (e == 0)
          active_camera_ = nullptr;

        ImGui::RadioButton(std::string(camera->to_string()).c_str(), &e, i+1);
        i++;
    }
    ImGui::End();
  DGUI_END;
}

void Scene::draw()
{
  this->debug_select_camera();

  if (active_camera_ == nullptr)
    return;

  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->draw(active_camera_);

    for (auto child = (*it)->iter_children(); child != (*it)->iter_children_end(); child++) {
      (*child)->draw(active_camera_);
    }
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

bool Scene::key_event(Window * window, int key, int action, int mods)
{
  if (active_camera_) {
    return active_camera_->key_event(window, key, action, mods);
  }

  return false;
}
bool Scene::mouse_event(Window * window, int key, int action, int mods)
{
  if (active_camera_) {
    return active_camera_->mouse_event(window, key, action, mods);
  }

  return false;
}

bool Scene::mouse_move_event(Window * window, double xpos, double ypos)
{
  if (active_camera_) {
    return active_camera_->mouse_move_event(window, xpos, ypos);
  }

  return false;
}

bool Scene::scroll_event(Window * window, double xoffset, double yoffset)
{
  if (active_camera_) {
    return active_camera_->scroll_event(window, xoffset, yoffset);
  }

  return false;
}

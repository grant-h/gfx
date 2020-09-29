#include "Scene.hpp"

#include <Log.hpp>
#include <CameraObject.hpp>
#include <Renderer.hpp>
#include <SceneRenderer.hpp>

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

std::shared_ptr<MultiLight> Scene::make_light(const char * name)
{
  auto light = std::make_shared<MultiLight>(name);

  lights_.push_back(light);
  add_object(light);

  return light;
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

  SceneRenderer renderer;

  // TODO: draw children of children (actually walk the scene graph)
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->draw(&renderer);

    for (auto child = (*it)->iter_children(); child != (*it)->iter_children_end(); child++) {
      (*child)->draw(&renderer);
    }
  }

  for (auto cmd : renderer.commands_) {
    
    cmd.shader->use();

    // TODO: let objects communicate this state change
    glEnable(GL_PROGRAM_POINT_SIZE);

    cmd.shader->set_uniform("M", cmd.obj->get_model_matrix());
    cmd.shader->set_uniform("MN", cmd.obj->get_normal_matrix());
    cmd.shader->set_uniform("V", active_camera_->get_view_matrix());
    cmd.shader->set_uniform("P", active_camera_->get_projection_matrix());
    cmd.shader->set_uniform("Camera", active_camera_->get_eye());

    cmd.shader->set_uniform("numLights", (int)lights_.size());

    int i = 0;
    for (auto light : lights_) {
      cmd.shader->set_uniform("lights", i, "type", (int)light->get_type());

      if (light->get_type() == MultiLight::LT_POINT) {
        cmd.shader->set_uniform("lights", i, "worldPos", light->position());
        cmd.shader->set_uniform("lights", i, "radius", light->get_radius());
      } else if (light->get_type() == MultiLight::LT_DIRECTIONAL) {
        cmd.shader->set_uniform("lights", i, "direction", light->get_direction());
      } else if (light->get_type() == MultiLight::LT_SPOTLIGHT) {
        cmd.shader->set_uniform("lights", i, "worldPos", light->position());
        cmd.shader->set_uniform("lights", i, "direction", light->get_direction());
        cmd.shader->set_uniform("lights", i, "cutoff", glm::cos(glm::radians(light->get_spotlight_cutoff())));
        cmd.shader->set_uniform("lights", i, "outerCutoff", glm::cos(glm::radians(light->get_spotlight_outer_cutoff())));
      }

      cmd.shader->set_uniform("lights", i, "color", light->get_color());
      cmd.shader->set_uniform("lights", i, "kAmbient", light->get_ambient());
      cmd.shader->set_uniform("lights", i, "kDiffuse", light->get_diffuse());
      cmd.shader->set_uniform("lights", i, "kSpecular", light->get_specular());
      i++;
    }

    if (cmd.texture_map && cmd.texture_map->texture_count()) {
      int texture_mode = 0;
      auto diffuse = cmd.texture_map->get_diffuse();
      auto specular = cmd.texture_map->get_specular();

      if (diffuse) {
        cmd.shader->set_uniform("TextureDiffuse", diffuse);
        texture_mode |= 1;
      }

      if (specular) {
        cmd.shader->set_uniform("TextureSpecular", specular);
        texture_mode |= 2;
      }

      cmd.shader->set_uniform("TextureMode", texture_mode);
    } else if (cmd.material) {
      cmd.shader->set_uniform("TextureMode", 0);
      cmd.shader->set_uniform("material.ambient", cmd.material->ambient);
      cmd.shader->set_uniform("material.diffuse", cmd.material->diffuse);
      cmd.shader->set_uniform("material.specular", cmd.material->specular);
      cmd.shader->set_uniform("material.shininess", cmd.material->shininess);
    }

    cmd.vao->draw();

    cmd.shader->unuse();
  }

  //LOG_INFO("%d draw commands", renderer.commands_.size());
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

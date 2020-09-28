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

  static PointLight gLight = { {0.0, 0.0, 0.0}, {0.8, 0.8, 1.0},
    {0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
  };

  static PointLight gLight2= { {0.0, 0.0, 0.0}, {0.8, 0.8, 1.0},
    {0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
    {0.5, 0.5, 0.5},
  };

  std::vector<PointLight *> lights = {&gLight, &gLight2};

  int i = 0;
  for (auto light : lights) {
    DGUI_BEGIN;
      ImGui::Begin((scene_name_ + " Light" + std::to_string(i)).c_str());
      ImGui::ColorEdit3("light color", (float*)glm::value_ptr(light->color));
      ImGui::ColorEdit3(" - ambient", (float*)glm::value_ptr(light->kAmbient));
      ImGui::ColorEdit3(" - diffuse", (float*)glm::value_ptr(light->kDiffuse));
      ImGui::ColorEdit3(" - specular", (float*)glm::value_ptr(light->kSpecular));
      ImGui::End();
    DGUI_END;

    i++;
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

    cmd.shader->set_uniform("numPointLights", (int)lights.size());

    int i = 0;
    for (auto light : lights) {
      cmd.shader->set_uniform("pointLights", i, "worldPos", light->position);
      cmd.shader->set_uniform("pointLights", i, "color", light->color);
      cmd.shader->set_uniform("pointLights", i, "kAmbient", light->kAmbient);
      cmd.shader->set_uniform("pointLights", i, "kDiffuse", light->kDiffuse);
      cmd.shader->set_uniform("pointLights", i, "kSpecular", light->kSpecular);
      i++;
    }

    if (cmd.texture_map && cmd.texture_map->texture_count()) {
      cmd.shader->set_uniform("TextureMode", 1);
      cmd.shader->set_uniform("Texture", 0, cmd.texture_map->get_albedo());
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

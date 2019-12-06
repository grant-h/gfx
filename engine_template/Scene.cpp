#include "Scene.hpp"

#include <Log.hpp>

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

void Scene::print_objects()
{
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    LOG_INFO("%s", (*it)->to_string().c_str());
  }
}

void Scene::tick()
{
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->tick();
  }
}

void Scene::draw(Viewport * viewport)
{
  for (auto it = objects_.begin(); it != objects_.end(); it++) {
    (*it)->draw(viewport);
  }
}

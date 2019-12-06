#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <vector>
#include <memory>
#include <string>

#include <SceneObject.hpp>

class Scene {
  public:
    Scene(const char * name);
    ~Scene();

    void add_object(SceneObject * obj);
    void add_object(std::shared_ptr<SceneObject> obj);
    void print_objects();

    void tick();
    void draw(Viewport * viewport);
  private:
    std::string scene_name_;
    std::vector<std::shared_ptr<SceneObject> > objects_;
};

#endif // _SCENE_HPP

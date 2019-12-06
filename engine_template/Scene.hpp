#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <vector>
#include <memory>
#include <string>

#include <SceneObject.hpp>

class Window;

class Scene {
  public:
    Scene(const char * name);
    ~Scene();

    void add_object(SceneObject * obj);
    void add_object(std::shared_ptr<SceneObject> obj);
    void set_camera(std::shared_ptr<CameraObject> camera);
    void print_objects();

    void tick();
    void draw();

    // events
    void resize(Window * window);
  private:
    std::shared_ptr<CameraObject> active_camera_;
    std::string scene_name_;
    std::vector<std::shared_ptr<SceneObject> > objects_;
};

#endif // _SCENE_HPP

#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <vector>
#include <memory>
#include <string>

#include <CameraController.hpp>

class Window;

class Scene {
  public:
    Scene(const char * name);
    ~Scene();

    void add_object(SceneObject * obj);
    void add_object(std::shared_ptr<SceneObject> obj);
    void set_camera(std::shared_ptr<CameraController> camera);

    // Debugging
    void print_objects();
    void debug_select_camera();

    void tick();
    void draw();

    // events
    void resize(Window * window);
    bool key_event(Window * window, int key, int action, int mods);
    bool mouse_event(Window * window, int key, int action, int mods);
    bool mouse_move_event(Window * window, double xpos, double ypos);
    bool scroll_event(Window * window, double xoffset, double yoffset);
  private:
    std::shared_ptr<CameraController> active_camera_;
    std::string scene_name_;
    std::vector<std::shared_ptr<SceneObject> > objects_;
};

#endif // _SCENE_HPP

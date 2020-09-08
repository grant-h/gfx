#ifndef _CAMERA_CONTROLLER_HPP
#define _CAMERA_CONTROLLER_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <CameraObject.hpp>

class Window;

class CameraController : public CameraObject {
  public:
    CameraController(const char * name);
    CameraController(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~CameraController() = default;

    virtual void tick() override;

    // events
    bool key_event(Window * window, int key, int action, int mods);
    bool mouse_event(Window * window, int key, int action, int mods);
    bool mouse_move_event(Window * window, double xpos, double ypos);
    bool scroll_event(Window * window, double xoffset, double yoffset);
  private:
    bool control_forward_, control_back_,
         control_left_, control_right_;
    bool has_mouse_;
    float yaw_start_, pitch_start_;
    glm::vec2 drag_position_;
};

#endif // _CAMERA_CONTROLLER_HPP

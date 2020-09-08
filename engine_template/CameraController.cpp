#include "CameraController.hpp"

#include <Renderer.hpp>
#include <Log.hpp>

CameraController::CameraController(const char * name)
  :CameraObject(name), control_forward_(false), control_back_(false), control_left_(false), control_right_(false), has_mouse_(false)
{
}

CameraController::CameraController(const char * name, std::shared_ptr<SceneObject> parent)
  :CameraObject(name, std::move(parent))
{
}


void CameraController::tick()
{
  glm::vec3 p = position();
  glm::vec3 forward = glm::normalize(position() - get_eye());
  glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
  glm::vec3 left = glm::normalize(glm::cross(up, forward));

  float sensitivity = 0.1f;

  if (control_forward_) {
    p += forward*sensitivity;
  }
  if (control_back_) {
    p -= forward*sensitivity;
  }
  if (control_left_) {
    p += left*sensitivity;
  }
  if (control_right_) {
    p -= left*sensitivity;
  }

  position(p);
}

bool CameraController::key_event(Window * window, int key, int action, int mods)
{
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch(key) {
      case GLFW_KEY_W: control_forward_ = true; break;
      case GLFW_KEY_A: control_left_ = true; break;
      case GLFW_KEY_S: control_back_ = true; break;
      case GLFW_KEY_D: control_right_ = true; break;
      default: return false;
    }

    return true;
  } else if (action == GLFW_RELEASE) {
    switch(key) {
      case GLFW_KEY_W: control_forward_ = false; break;
      case GLFW_KEY_A: control_left_ = false; break;
      case GLFW_KEY_S: control_back_ = false; break;
      case GLFW_KEY_D: control_right_ = false; break;
      default: return false;
    }

    return true;
  }

  return false;
}

bool CameraController::mouse_event(Window * window, int key, int action, int mods)
{
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch(key) {
      case GLFW_MOUSE_BUTTON_LEFT:
        has_mouse_ = true;

        int sizes[2];
        double mousePos[2];

        glfwGetCursorPos(window->get_glfw_window(), &mousePos[0], &mousePos[1]);
        glfwGetWindowSize(window->get_glfw_window(), &sizes[0], &sizes[1]);

        drag_position_ = glm::vec2(mousePos[0]/sizes[0], mousePos[1]/sizes[1]);

        yaw_start_ = get_yaw();
        pitch_start_ = get_pitch();

        LOG_INFO("%.2f, %.2f", drag_position_.x, drag_position_.y);
        break;
      default: return false;
    }

    return true;
  } else if (action == GLFW_RELEASE) {
    switch(key) {
      case GLFW_MOUSE_BUTTON_LEFT: has_mouse_ = false; break;
      default: return false;
    }

    return true;
  }

  return false;
}

bool CameraController::mouse_move_event(Window * window, double xpos, double ypos)
{
  if (!has_mouse_)
    return false;

  int sizes[2];
  double mousePos[2];

  glfwGetCursorPos(window->get_glfw_window(), &mousePos[0], &mousePos[1]);
  glfwGetWindowSize(window->get_glfw_window(), &sizes[0], &sizes[1]);

  glm::vec2 next = glm::vec2(mousePos[0]/sizes[0], mousePos[1]/sizes[1]);
  glm::vec2 delta = next - drag_position_;

  set_yaw(yaw_start_ - delta.x*3.0);
  set_pitch(pitch_start_ + delta.y*3.0);

  return true;
}

bool CameraController::scroll_event(Window * window, double xoffset, double yoffset)
{
  if (!has_mouse_)
    return false;

  return true;
}

#include "Window.hpp"

#include <Log.hpp>

#include <chrono>
#include <thread>

Window::Window(const char * title)
  :window_title_(title), window_width_(0), window_height_(0), exit_loop_(false), last_fps_(0)
{
}

Window::~Window()
{
}

bool Window::create(int width, int height)
{
  created_ = false;

  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  glfwDefaultWindowHints();

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window_width_ = width;
  window_height_ = height;

  // Open a window and create its OpenGL context
  window_ = glfwCreateWindow(window_width_, window_height_, window_title_.c_str(), NULL, NULL);

  if (window_ == nullptr) {
    LOG_ERROR("Failed to open GLFW window.");
    glfwTerminate();
    return false;
  }

  // Set up inputs
  glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_FALSE);

  // Grab the framebuffer size as it may be different from the window
  glfwGetFramebufferSize(window_, &fb_width_, &fb_height_);

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    LOG_ERROR("Failed to initialize GLEW");
    return false;
  }

  glfwSetWindowUserPointer(window_, this);

  // Enable mouse & keyboard input
  glfwSetMouseButtonCallback(window_, mouse_cb);
  glfwSetKeyCallback(window_, keyboard_cb);
  glfwSetScrollCallback(window_, scroll_cb);
  glfwSetCursorPosCallback(window_, mouse_move_cb);

  // Enable window callbacks for dynamic framebuffer sizing
  glfwSetWindowSizeCallback(window_, resize_window_cb);
  glfwSetFramebufferSizeCallback(window_, resize_fb_cb);
  glfwSetWindowFocusCallback(window_, focus_window_cb);

  LOG_INFO("Created Window<%dx%d> (fb %dx%d)", width, height, fb_width_, fb_height_);

  created_ = true;

  return true;
}

void Window::set_scene(std::shared_ptr<Scene> scene)
{
  current_scene_ = scene;
}


int Window::get_width()
{
  return fb_width_;
}

int Window::get_height()
{
  return fb_height_;
}

int Window::get_fps()
{
  return last_fps_;
}

void Window::process()
{
  double last_time = glfwGetTime();
  double start_time = last_time;
  int num_frames = 0;

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  // Enable scissoring for viewport clearing
  glEnable(GL_SCISSOR_TEST);

  while (!exit_loop_ && glfwWindowShouldClose(window_) == 0)
  {
    double current_time = glfwGetTime();
    num_frames++;

    if (current_time - last_time >= 1.0) {
      last_fps_ = num_frames;
      num_frames = 0;

      printf("FPS %d\n", last_fps_);
      last_time = current_time;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    current_scene_->tick();
    current_scene_->draw();

    glfwSwapBuffers(window_);

    // Prevent no vsync when minimized (C++11)
    if (!glfwGetWindowAttrib(window_, GLFW_FOCUSED)) {
       std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
}

////////////////////////////////

void Window::keyboard_cb(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  Window * win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  const char * key_name = glfwGetKeyName(key, scancode);

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        win->exit_loop_ = true;
        return;
      default:
        LOG_WARN("Unhandled key '%s' (%d,%d)",
            key_name, key, scancode);
        break;
    }
  }
}

void Window::mouse_cb(GLFWwindow* window, int button, int action, int mods)
{
  LOG_TRACE_FUNC("ev");
}

void Window::mouse_move_cb(GLFWwindow* window, double xpos, double ypos)
{
  LOG_TRACE_FUNC("ev");
}

void Window::scroll_cb(GLFWwindow* window, double xoffset, double yoffset)
{
  LOG_TRACE_FUNC("ev");
}

void Window::resize_window_cb(GLFWwindow* window, int width, int height)
{
  LOG_TRACE_FUNC("ev");
}

void Window::resize_fb_cb(GLFWwindow* window, int width, int height)
{
  Window * win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  glfwGetFramebufferSize(window, &win->fb_width_, &win->fb_height_);

  if (win->current_scene_) {
    win->current_scene_->resize(win);
  }
}

void Window::focus_window_cb(GLFWwindow* window, int focused)
{
  LOG_TRACE_FUNC("ev");
}
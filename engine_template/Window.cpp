#include "Window.hpp"

#include <Log.hpp>
#include <ResourceManager.hpp>

#include <chrono>
#include <thread>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <DebugEditor.hpp>

Window::Window(const char * title)
  :window_title_(title), window_width_(0), window_height_(0), last_fps_(0), debug_menu_(false)
{
}

Window::~Window()
{
  if (created_) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  LOG_DEBUG("Window destroyed");
}

bool Window::create(int width, int height)
{
  created_ = false;

  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return false;
  }

  glfwDefaultWindowHints();

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);

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
    glfwDestroyWindow(window_);
    glfwTerminate();
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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

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
  double last_all_time = 0.0, all_time = 0.0, events_time = 0.0,
         clear_time = 0.0, tick_time = 0.0,
         draw_time = 0.0, swap_time = 0.0;

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  //glEnable(GL_CULL_FACE);
  // Enable scissoring for viewport clearing
  glEnable(GL_SCISSOR_TEST);

  bool show_demo_window = false;
  static ImVec4 clear_color(0.0f, 0.0f, 0.0f, 1.0f);

  ResourceManager * res = ResourceManager::instance();

  DebugEditor editor;

  while (!glfwWindowShouldClose(window_))
  {
    all_time = glfwGetTime();

    events_time = glfwGetTime();
    {
      glfwPollEvents();
      res->process_events();
    }
    events_time = glfwGetTime() - events_time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ////////////////////////////

    clear_time = glfwGetTime();
    {
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    clear_time = glfwGetTime() - clear_time;

    glEnable(GL_DEPTH_TEST);

    if (current_scene_) {
      tick_time = glfwGetTime();
      current_scene_->tick();
      tick_time = glfwGetTime() - tick_time;
    }

    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Begin("EngineTemplate")) {
      ImGui::Checkbox("Demo Window", &show_demo_window);
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      double at = last_all_time/100.0;
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Text("CPU Utilization: draw %3.1f/%3.1f%%, tick %3.1f/%3.1f%%, clear %3.1f/%3.1f%%, event %3.1f/%3.1f%%",
          draw_time*1000.0, draw_time/at, tick_time*1000.0, tick_time/at, clear_time*1000.0, clear_time/at, events_time*1000.0, events_time/at);
      ImGui::Text("GPU Utilization: swap %3.1f/%3.1f%%", swap_time*1000.0, swap_time/at);
    }

    ImGui::End();

    draw_time = glfwGetTime();

    editor.draw();

    if (current_scene_) {
      current_scene_->draw();
    }

    ////////////////////////////

    if (debug_menu_) {
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    } else {
      ImGui::EndFrame();
    }

    draw_time = glfwGetTime() - draw_time;

    swap_time = glfwGetTime();
    {
      glfwSwapBuffers(window_);
    }
    swap_time = glfwGetTime() - swap_time;

    // Prevent no vsync when minimized (C++11)
    if (!glfwGetWindowAttrib(window_, GLFW_FOCUSED)) {
       std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    last_all_time = glfwGetTime() - all_time;
  }
}

////////////////////////////////

void Window::keyboard_cb(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  Window * win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        return;
      case GLFW_KEY_GRAVE_ACCENT:
        win->debug_menu_ = !win->debug_menu_;
        return;
    }

    // the special "escape" keys are always handled by the application
    if (ImGui::GetIO().WantCaptureKeyboard)
      return;

    switch (key)
    {
      default: {
        const char * key_name = glfwGetKeyName(key, scancode);
        LOG_WARN("Unhandled key '%s' (%d,%d)",
            key_name, key, scancode);
        break;
     }
    }
  }
}

void Window::mouse_cb(GLFWwindow* window, int button, int action, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  LOG_TRACE_FUNC("ev");
}

void Window::mouse_move_cb(GLFWwindow* window, double xpos, double ypos)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  //LOG_TRACE_FUNC("ev");
}

void Window::scroll_cb(GLFWwindow* window, double xoffset, double yoffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  //LOG_TRACE_FUNC("ev");
}

void Window::resize_window_cb(GLFWwindow* window, int width, int height)
{
  //LOG_TRACE_FUNC("ev");
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
  //LOG_TRACE_FUNC("ev");
}

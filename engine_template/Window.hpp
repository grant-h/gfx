#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include <Scene.hpp>

class Window {
  public:
    Window(const char * title);
    ~Window();

    bool create(int width, int height);
    void set_scene(std::shared_ptr<Scene> scene);
    int get_width();
    int get_height();
    int get_fps();
    void process();
  private:
    static void keyboard_cb(GLFWwindow * window, int key, int scancode, int action, int mods);
    static void mouse_cb(GLFWwindow* window, int button, int action, int mods);
    static void mouse_move_cb(GLFWwindow* window, double xpos, double ypos);
    static void scroll_cb(GLFWwindow* window, double xoffset, double yoffset);
    static void resize_window_cb(GLFWwindow* window, int width, int height);
    static void resize_fb_cb(GLFWwindow* window, int width, int height);
    static void focus_window_cb(GLFWwindow* window, int focused);
  private:
    GLFWwindow * window_;
    std::string window_title_;

    std::shared_ptr<Scene> current_scene_;
    bool created_;
    bool exit_loop_;
    int window_width_, window_height_;
    int fb_width_, fb_height_;
    int last_fps_;
};

#endif // _WINDOW_HPP

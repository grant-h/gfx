#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <mutex>

#include <Shader.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>
#include <VertexArray.hpp>
#include <monitor.hpp>

class ResourceManager {
  public:
    ResourceManager();
    ~ResourceManager();
    static ResourceManager * instance();

    std::string get_resource_path();

    // Textures
    std::shared_ptr<Texture> get_texture(std::string name);
    bool create_texture(const std::string & name, const std::string filename);

    // Shaders
    std::string get_shader_path();
    bool create_program(const std::string & name, const std::vector<std::string> & shaders);
    std::shared_ptr<ShaderProgram> get_shader_program(std::string name);
    std::shared_ptr<Shader> get_shader(std::string name);
    std::vector<std::shared_ptr<Shader>> get_shaders();
    void recompile_shader(std::shared_ptr<Shader> shader);

    // Models
    std::string get_model_path();

    void process_events();
    void watch_shaders();
  private:
    std::shared_ptr<Shader> create_shader(std::string path);

    std::map<std::string, std::shared_ptr<Texture>> res_textures_;
    std::map<std::string, std::shared_ptr<Shader>> res_shaders_;
    std::map<std::string, std::shared_ptr<ShaderProgram>> res_shader_programs_;

    static void monitor_events(const std::vector<fsw::event> & events, void * ptr);
    std::mutex io_mutex_;
    std::vector<fsw::event> queued_events_;
    std::unique_ptr<std::thread> file_monitor_thread_;
    std::unique_ptr<fsw::monitor> file_monitor_;
};

#endif // _RESOURCE_MANAGER_H

#include <ResourceManager.hpp>

#include <Log.hpp>
#include <LoadObj.hpp>
#include <monitor_factory.hpp>
#include <set>
#include <fstream>

static std::unique_ptr<ResourceManager> g_ResourceManager = nullptr;

static bool ends_with(const std::string& str, const std::string& suffix)
{
  return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

std::string basename(std::string & path)
{
    std::string new_path = path;
    auto it = path.rfind("/");
    return new_path.substr(it+1);
}

ResourceManager::ResourceManager()
{
  add_resource_path("data/"); // local resources
  add_resource_path("../data/"); // shared resources (can be overriden by local)
}

ResourceManager::~ResourceManager()
{
  if (file_monitor_ && file_monitor_->is_running()) {
    file_monitor_->stop();
    file_monitor_thread_->join();
  }
}

std::vector<std::string> ResourceManager::get_resource_paths()
{
  return res_paths_;
}

void ResourceManager::add_resource_path(const std::string & path)
{
  // TODO: don't add duplicate paths
  res_paths_.push_back(path);
}

std::string ResourceManager::find_first_file(const std::string & filename) {
  // TODO: do this in a portable and efficient way (stat() && CreateFile)
  for (auto it = res_paths_.begin(); it != res_paths_.end(); it++) {
    std::string path = *it;

    // TODO: UNIX vs windows paths
    if (path.at(path.length()-1) != '/')
      path += "/";

    path += filename;

    std::ifstream f(path);

    if (f.is_open()) {
      return path;
    }
  }

  return "";
}

std::shared_ptr<Texture> ResourceManager::get_texture(std::string name)
{
  auto it = res_textures_.find(name);

  LOG_FATAL_ASSERT(it != res_textures_.end(), "Unable to find required texture %s", name.c_str());

  return it->second;
}

bool ResourceManager::create_texture(const std::string & name, const std::string filename)
{
  LOG_FATAL_ASSERT(res_textures_.find(name) == res_textures_.end(), "Non-unique texture created: %s", name.c_str());

  auto texture = std::make_shared<Texture>(name);

  std::string path = find_first_file(filename);

  if (path == "") {
    LOG_ERROR("Unable to find texture file '%s'", filename.c_str());
    return false;
  }

  if (!texture->load_from_file(path)) {
    return false;
  }

  res_textures_[name] = texture;

  return true;
}

bool ResourceManager::create_program(const std::string & name, const std::vector<std::string> & shaders)
{
  LOG_FATAL_ASSERT(res_shader_programs_.find(name) == res_shader_programs_.end(), "Non-unique shader program created: %s", name.c_str());

  auto program = std::make_shared<ShaderProgram>(name);

  for (auto it = shaders.begin(); it != shaders.end(); it++) {
    std::string path = *it;
    std::shared_ptr<Shader> shader;

    if (res_shaders_.find(path) != res_shaders_.end())
      shader = res_shaders_[path];
    else
      shader = create_shader(path);

    if (!shader) {
      return false;
    }

    program->add_shader(shader);
  }

  if (!program->link())
    return false;

  res_shader_programs_[name] = program;

  return true;
}

std::shared_ptr<ShaderProgram> ResourceManager::get_shader_program(std::string name)
{
  auto it = res_shader_programs_.find(name);

  LOG_FATAL_ASSERT(it != res_shader_programs_.end(), "Unable to find required shader program %s", name.c_str());

  return it->second;
}

std::vector<std::shared_ptr<Shader>> ResourceManager::get_shaders()
{
  std::vector<std::shared_ptr<Shader>> shaders;
  for (auto && shader : res_shaders_)
    shaders.push_back(shader.second);

  return shaders;
}

std::shared_ptr<Shader> ResourceManager::get_shader(std::string name)
{
  auto it = res_shaders_.find(name);

  LOG_FATAL_ASSERT(it != res_shaders_.end(), "Unable to find required shader %s", name.c_str());

  return it->second;
}

std::shared_ptr<Shader> ResourceManager::create_shader(std::string filename)
{
  int type = -1;

  if (ends_with(filename, ".frag")) {
    type = GL_FRAGMENT_SHADER;
  } else if (ends_with(filename, ".vert")) {
    type = GL_VERTEX_SHADER;
  } else if (ends_with(filename, ".geom")) {
    type = GL_GEOMETRY_SHADER;
  } else {
    LOG_FATAL("Unknown shader type from filename %s", filename.c_str());
  }

  std::string path = find_first_file("shader/" + filename);

  if (path == "") {
    LOG_ERROR("Unable to find shader file '%s'", filename.c_str());
    return nullptr;
  }

  assert(res_shaders_.find(path) == res_shaders_.end());

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(type);

  if (!shader->load_from_file(path))
    return nullptr;

  // TODO: canonicalize the path/filename
  res_shaders_[filename] = shader;

  return shader;
}


void ResourceManager::watch_shaders()
{
  using namespace fsw;

  std::vector<std::string> shader_paths;

  for (auto it = res_paths_.begin(); it != res_paths_.end(); it++) {
    shader_paths.push_back(*it + "shader/");
  }

  file_monitor_ = std::unique_ptr<monitor>(
      monitor_factory::create_monitor(
        fsw_monitor_type::system_default_monitor_type,
        shader_paths,
        ResourceManager::monitor_events,
        this)
      );

  file_monitor_->set_latency(0.5);

  file_monitor_thread_ = std::unique_ptr<std::thread>(new std::thread([this](){
      LOG_DEBUG("File monitor starting");
      file_monitor_->start();
      LOG_DEBUG("File monitor ending");
  }));
}

void ResourceManager::recompile_shader(std::shared_ptr<Shader> shader)
{
  std::set<std::shared_ptr<ShaderProgram>> updated_programs;

  if (!shader->compile())
    return;

  // find all already-linked programs that use this code
  // and relink them
  for (auto program_it = res_shader_programs_.begin();
      program_it != res_shader_programs_.end(); program_it++) {
    if (program_it->second->has_shader(shader))
      updated_programs.insert(program_it->second);
  }

  for (auto it = updated_programs.begin(); it != updated_programs.end(); it++) {
    if (!(*it)->link()) {
      //LOG_ERROR("failed to link");
    }
  }
}

bool ResourceManager::create_model(const std::string & name, const std::string filename)
{
  LOG_FATAL_ASSERT(res_vao_.find(name) == res_vao_.end(), "Non-unique model created: %s", name.c_str());

  std::string path = find_first_file("model/" + filename);

  if (path == "") {
    LOG_ERROR("Unable to find model file '%s'", filename.c_str());
    return false;
  }

  auto model = std::shared_ptr<VertexArray>(load_obj(path.c_str()));

  if (!model) {
    return false;
  }

  res_vao_[name] = model;

  return true;
}

std::shared_ptr<VertexArray> ResourceManager::get_model(const std::string & name)
{
  auto it = res_vao_.find(name);

  LOG_FATAL_ASSERT(it != res_vao_.end(), "Unable to find required model %s", name.c_str());

  return it->second;
}

void ResourceManager::process_events()
{
  std::lock_guard<std::mutex> lk(io_mutex_);

  for (auto it = queued_events_.begin(); it != queued_events_.end(); it++) {
    std::string path = std::move((*it).get_path());
    std::vector<fsw_event_flag> flags = (*it).get_flags();
    std::string base = basename(path);

    bool we_care = false;

    auto found = res_shaders_.find(base);

    // if the file event is not for a shader we have, discard
    if (found == res_shaders_.end())
      continue;

    for (auto && flag : flags) {
      //LOG_INFO("File Event: %s (%s)", base.c_str(), fsw::event::get_event_flag_name(flag).c_str());
      if (flag == Updated)
        we_care = true;
    }

    if (!we_care)
      continue;

    if (found->second->reload()) {
      recompile_shader(found->second);
    }
  }

  queued_events_.clear();
}

void ResourceManager::monitor_events(const std::vector<fsw::event> & events, void * ptr)
{
  ResourceManager * res = reinterpret_cast<ResourceManager*>(ptr);
  std::lock_guard<std::mutex> lk(res->io_mutex_);

  for (auto it = events.begin(); it != events.end(); it++) {
    res->queued_events_.push_back(*it);
    //LOG_INFO("OUTER File Event: %s", (*it).get_path().c_str());
  }
}

ResourceManager * ResourceManager::instance()
{
  if (!g_ResourceManager) {
    g_ResourceManager = std::unique_ptr<ResourceManager>(new ResourceManager());
    assert(g_ResourceManager);
  }

  return g_ResourceManager.get();
}

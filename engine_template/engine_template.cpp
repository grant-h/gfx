#include <memory>

#include "Log.hpp"
#include "Window.hpp"
#include <Scene.hpp>
#include <SceneObject.hpp>
#include <ShaderProgram.hpp>
#include <PointObject.hpp>
#include <CameraObject.hpp>

#include <random>

static std::string getResource(const char * path) {
   return std::string("data/") + std::string(path);
}
static std::string getResource(std::string path) {
   return getResource(path.c_str());
}
static std::string getShader(const char * path) {
   return getResource(std::string("shader/") + std::string(path));
}

int main(int argc, char * argv[])
{
  log_set_level(LOG_LEVEL_TRACE);

  if (argc > 0)
    LOG_INFO("Starting %s", argv[0]);

  auto window = std::unique_ptr<Window>(new Window("EngineTemplate"));

  if (!window->create(1280, 720)) {
    return 1;
  }

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(-10.0, 10.0);
  std::uniform_real_distribution<> dis_color(0.0, 1.0);

  auto s1 = std::make_shared<Shader>(GL_VERTEX_SHADER);
  if (!s1->load_from_file(getShader("Point.vert").c_str())) {
    return 1;
  }
  auto s2 = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
  if (!s2->load_from_file(getShader("Point.frag").c_str())) {
    return 1;
  }

  auto sp1 = std::make_shared<ShaderProgram>();
  sp1->add_shader(s1);
  sp1->add_shader(s2);
  if (!sp1->link())
    return 1;

  auto scene = std::make_shared<Scene>("main");
  auto camera = std::make_shared<CameraObject>("camera1");

  if (!camera->init()) {
      LOG_ERROR("Camera init fail");
      return 1;
  }

  scene->set_camera(camera);
  scene->add_object(camera);

  for (int i = 0; i < 500; i++) {
    auto point1 = std::make_shared<PointObject>("point1");
    point1->set_shader(sp1);

    point1->set_color(dis_color(gen), dis_color(gen), dis_color(gen));

    if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
    }

    point1->position(dis(gen), dis(gen), dis(gen));

    scene->add_object(point1);
  }
  scene->print_objects();

  window->set_scene(scene);
  window->process();
}

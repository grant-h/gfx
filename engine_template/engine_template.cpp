#include <memory>

#include "Log.hpp"
#include "Window.hpp"
#include <Scene.hpp>
#include <SceneObject.hpp>
#include <PointObject.hpp>

#include <random>

int main(int argc, char * argv[])
{
  log_set_level(LOG_LEVEL_TRACE);

  if (argc > 0)
    LOG_INFO("Starting %s", argv[0]);

  Window * window = new Window("EngineTemplate");

  if (!window->create(640, 480)) {
    return 1;
  }

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(-5.0, 2.0);

  auto scene = std::make_shared<Scene>("main");
  for (int i = 0; i < 50; i++) {
    auto point1 = std::make_shared<PointObject>("point1");

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

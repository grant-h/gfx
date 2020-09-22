#include <memory>

#include <random>

#include <Log.hpp>
#include <Window.hpp>
#include <ResourceManager.hpp>

#include <Scene.hpp>
#include <SceneObject.hpp>
#include <ShaderProgram.hpp>
#include <PointObject.hpp>
#include <CameraController.hpp>
#include <Cube.hpp>
#include <LoadObj.hpp>

int main(int argc, char * argv[])
{
  log_set_level(LOG_LEVEL_INFO);

  if (argc > 0)
    LOG_INFO("Starting %s", argv[0]);

  if (argc > 1)
    log_set_level(LOG_LEVEL_DEBUG);

  ResourceManager * res = ResourceManager::instance();
  auto window = std::unique_ptr<Window>(new Window("EngineTemplate"));

  if (!window->create(1280, 720)) {
    return 1;
  }

  load_obj((res->get_model_path() + "cube.obj").c_str());

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(-10.0, 10.0);
  std::uniform_real_distribution<> dis_unit(-1.0, 1.0);
  std::uniform_real_distribution<> dis_unit_one(0.0, 1.0);
  std::uniform_real_distribution<> dis_color(0.0, 1.0);

  struct Program {
    std::string name;
    std::vector<std::string> shaders;
  };

  const Program programs[] = {
    { "Point", {"Point.vert", "Point.frag"} },
    { "Simple", {"Simple.vert", "Simple.frag"} },
    { "Phong", {"Phong.vert", "Phong.frag"} },
    { "Debug", {"Debug.vert", "Debug.geom", "Debug.frag"} },
  };

  for (int i = 0; i < sizeof(programs)/sizeof(*programs); i++) {
    const Program * p = &programs[i];

    if (!res->create_program(p->name, p->shaders))
      return 1;
  }

  if (!res->create_texture("container", "image/container.jpg")) {
    return 1;
  }

  if (!res->create_texture("roombox", "image/roombox.png")) {
    return 1;
  }

  if (!res->create_texture("uv_debug", "image/checker-map_tho.png")) {
    return 1;
  }

  res->watch_shaders();

  auto scene = std::make_shared<Scene>("main");
  auto camera = std::make_shared<CameraController>("camera1");

  if (!camera->init()) {
      LOG_ERROR("Camera init fail");
      return 1;
  }

  /*for (float x = 0.0; x < 40.0; x += 2.0) { 
    for (float y = 0.0; y < 40.0; y += 2.0) { 
    auto mesh1 = std::make_shared<Cube>("cube1");

    if (!mesh1->init()) {
      LOG_ERROR("Mesh init failed");
      return 1;
    }

    mesh1->position(x, y, 0.0);
    mesh1->set_scale(0.5);
    scene->add_object(mesh1);
    }
  }*/
  auto mesh1 = std::make_shared<Cube>("cube1");
  mesh1->init();
  mesh1->position(0.0, 5.0, 0.0);
  mesh1->set_scale(5.0);
  scene->add_object(mesh1);

  camera->position(0.0, 2.5, 0.0);

  scene->set_camera(camera);
  scene->add_object(camera);

  auto point1 = std::make_shared<PointObject>("origin");
  point1->position(0.0, 0.0, 0.0);
  point1->set_color(1.0, 1.0, 1.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("x-axis");
  point1->position(1.0f, 0.0, 0.0);
  point1->set_color(1.0f, 0.0, 0.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("y-axis");
  point1->position(0.0, 1.0f, 0.0);
  point1->set_color(0.0, 1.0f, 0.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("z-axis");
  point1->position(0.0, 0.0, 1.0);
  point1->set_color(0.0, 0.0, 1.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);
  

  for (int i = 0; i < 400; i++) {
    auto point1 = std::make_shared<PointObject>("point1");

    point1->set_color(1.0- dis_unit_one(gen)*0.1, 1.0- dis_unit_one(gen)*0.1, 1.0 - dis_unit_one(gen)*0.1);

    if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return 1;
    }

    //point1->set_shader(sp1);

    //point1->set_color(dis_color(gen), dis_color(gen), dis_color(gen));
    glm::vec3 dir(dis_unit(gen), dis_unit(gen), dis_unit(gen));
    //glm::vec3 dir(0.0, 0.0, 0.0);
    dir = glm::normalize(dir)*400.0f;
    point1->position(dir);

    scene->add_object(point1);
  }

  scene->print_objects();

  window->set_scene(scene);
  window->process();
}

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
#include <Mesh.hpp>
#include <LoadObj.hpp>

enum resource_type {
  RT_SHADER,
  RT_TEXTURE,
  RT_MODEL,
};

struct resource_entry {
  resource_type type;
  std::string name;
  std::vector<std::string> files;
};

static resource_entry RESOURCE_MAP[] = {
  // Shaders
  {RT_SHADER, "Point", {"Point.vert", "Point.frag"} },
  {RT_SHADER, "Simple", {"Simple.vert", "Simple.frag"} },
  {RT_SHADER, "Phong", {"Phong.vert", "Phong.frag"} },
  {RT_SHADER, "Debug", {"Debug.vert", "Debug.geom", "Debug.frag"} },

  // Textures
  {RT_TEXTURE, "uv_debug", {"image/checker-map_tho.png"} },
  {RT_TEXTURE, "container2", {"image/container2.png"} },
  {RT_TEXTURE, "container2-specular", {"image/container2_specular.png"} },

  // Models
  {RT_MODEL, "debug-camera", {"camera.obj"} },
  {RT_MODEL, "invcube", {"invcube.obj"} },
  {RT_MODEL, "cube", {"cube.obj"} },
  {RT_MODEL, "cubeuv", {"cube_same_uv.obj"} },
  {RT_MODEL, "room", {"test-room.obj"} },
  {RT_MODEL, "lowpoly", {"lowpoly.obj"} },
};

bool init_resources();
std::shared_ptr<Scene> build_scene();

int main(int argc, char * argv[])
{
  log_set_level(LOG_LEVEL_INFO);

  if (argc > 0)
    LOG_INFO("Starting %s", argv[0]);

  if (argc > 1)
    log_set_level(LOG_LEVEL_DEBUG);

  auto window = std::unique_ptr<Window>(new Window("SpinningCube"));

  if (!window->create(1280, 720)) {
    return 1;
  }

  if (!init_resources())
    return 1;

  auto scene = build_scene();

  if (!scene)
    return 1;

  scene->print_objects();

  window->set_scene(scene);
  window->process();
}

std::shared_ptr<Scene> build_scene()
{
  ResourceManager * res = ResourceManager::instance();

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  //std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::mt19937 gen(28882); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(-10.0, 10.0);
  std::uniform_real_distribution<> dis_unit(-1.0, 1.0);
  std::uniform_real_distribution<> dis_unit_one(0.0, 1.0);
  std::uniform_real_distribution<> dis_color(0.0, 1.0);

  auto scene = std::make_shared<Scene>("main");
  auto camera = std::make_shared<CameraController>("camera1");
  auto camera2 = std::make_shared<CameraController>("camera2");
  auto light = scene->make_light("light1");
  auto light2 = scene->make_light("light2");

  light->make_point_light({-2.0, 3.0, -2.0}, {1.0, 1.0, 1.0}, 2.0);
  light2->make_point_light({2.0, 2.0, 2.0}, {0.5, 0.5, 1.0}, 3.0);

  /*
  auto sun = scene->make_light("light_sun");
  sun->make_directional_light({1.0, -1.0, 1.0}, {1.0, 1.0, 0.9});
  sun->set_specular(0.1);
  sun->set_ambient(0.1);*/

  auto flashlight = scene->make_light("flashlight");
  flashlight->make_spotlight({0.0, 3.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 12.5f, 14.5f);

  if (!camera->init()) {
      LOG_ERROR("Camera init fail");
      return nullptr;
  }

  if (!camera2->init()) {
      LOG_ERROR("Camera init fail");
      return nullptr;
  }


  auto box_tex = std::make_shared<TextureMap>();

  box_tex->set_diffuse(res->get_texture("container2"));
  box_tex->set_specular(res->get_texture("container2-specular"));

  for (int i = 0; i < 30; i++) {
    auto box = std::make_shared<Mesh>(("box" + std::to_string(i)).c_str());
    box->set_shader(res->get_shader_program("Phong"));
    box->set_geometry(res->get_model("cubeuv"));
    box->set_textures(box_tex);

    float scale = 0.5 + dis_unit_one(gen)*1.0;
    auto rot = box->get_rotation();
    rot.y += dis_unit(gen);

    box->position(2.5*(int)dis(gen), scale, 2.0*(int)dis(gen));
    box->set_rotation(rot);
    box->set_scale(scale);

    scene->add_object(box);
  }


  auto room_tex = std::make_shared<TextureMap>();
  room_tex->set_diffuse(res->get_texture("uv_debug"));

  auto mesh1 = std::make_shared<Mesh>("cube1");
  mesh1->position(0.0, 10.0, 0.0);
  mesh1->set_scale(20.0, 10.0, 20.0);

  mesh1->set_shader(res->get_shader_program("Phong"));
  mesh1->set_geometry(res->get_model("invcube"));
  mesh1->set_textures(room_tex);

  scene->add_object(mesh1);

  camera->position(0.0, 2.5, 0.0);
  camera2->position(0.0, 1.0, 0.0);

  scene->set_camera(camera);
  scene->add_object(camera);
  scene->add_object(camera2);

  auto point1 = std::make_shared<PointObject>("origin");
  point1->position(0.0, 0.0, 0.0);
  point1->set_color(1.0, 1.0, 1.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return nullptr;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("x-axis");
  point1->position(1.0f, 0.0, 0.0);
  point1->set_color(1.0f, 0.0, 0.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return nullptr;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("y-axis");
  point1->position(0.0, 1.0f, 0.0);
  point1->set_color(0.0, 1.0f, 0.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return nullptr;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);

  point1 = std::make_shared<PointObject>("z-axis");
  point1->position(0.0, 0.0, 1.0);
  point1->set_color(0.0, 0.0, 1.0);
  if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return nullptr;
  }
  //point1->set_shader(sp1);
  scene->add_object(point1);
  

  for (int i = 0; i < 0; i++) {
    auto point1 = std::make_shared<PointObject>("point1");

    point1->set_color(1.0- dis_unit_one(gen)*0.1, 1.0- dis_unit_one(gen)*0.1, 1.0 - dis_unit_one(gen)*0.1);

    if (!point1->init()) {
      LOG_ERROR("Point init fail");
      return nullptr;
    }

    //point1->set_shader(sp1);

    //point1->set_color(dis_color(gen), dis_color(gen), dis_color(gen));
    glm::vec3 dir(dis_unit(gen), dis_unit(gen), dis_unit(gen));
    //glm::vec3 dir(0.0, 0.0, 0.0);
    dir = glm::normalize(dir)*400.0f;
    point1->position(dir);

    scene->add_object(point1);
  }

  return scene;
}

bool init_resources()
{
  ResourceManager * res = ResourceManager::instance();

  for (int i = 0; i < sizeof(RESOURCE_MAP)/sizeof(*RESOURCE_MAP); i++) {
    const resource_entry * e = &RESOURCE_MAP[i];

    if (e->type == RT_SHADER) {
      if (!res->create_program(e->name, e->files))
        return false;
    } else if (e->type == RT_TEXTURE) {
      if (!res->create_texture(e->name, e->files[0]))
        return false;
    } else if (e->type == RT_MODEL) {
      if (!res->create_model(e->name, e->files[0]))
        return false;
    }
  }

  res->watch_shaders();

  return true;
}

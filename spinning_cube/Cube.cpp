#include <Cube.hpp>

#include <MeshMaker.hpp>
#include <ResourceManager.hpp>
#include <LoadObj.hpp>

Cube::Cube(const char * name)
  :SceneObject(name)
{
}

Cube::~Cube()
{
}

bool Cube::init()
{
  vao_ = ResourceManager::instance()->get_model("invcube");
  shader_ = ResourceManager::instance()->get_shader_program("Phong");
  texture1_ = ResourceManager::instance()->get_texture("uv_debug");

  return true;
}

void Cube::tick()
{
  glm::vec3 rot = get_rotation();
  rot.x = 0.5 + glfwGetTime()*1.1;
  rot.y = 2.5 + glfwGetTime()*0.7;
//  set_rotation(rot);
}

void Cube::draw(std::shared_ptr<CameraObject> camera)
{
  shader_->use();

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("Time", (float)glfwGetTime());
  shader_->set_uniform("Camera", camera->get_eye());

  static float f1 = 0.0f, f2=0.2f;
  static ImVec4 light_color(0.8f, 0.8f, 1.0f, 1.0f);
  static float mod = 1.0f;
  static bool usetex = true;
  ImVec4 light_pos(sin(glfwGetTime())*2.0f, 0.5f,cos(glfwGetTime())* 2.0f, 0.0f);

  DGUI_BEGIN
    ImGui::Begin("Slider");
    ImGui::Checkbox("usetex", &usetex);
    ImGui::SliderFloat("distance", &f1, -1.0f, 20.0f, "distance = %.3f");
    ImGui::SliderFloat("falloff", &f2, 0.0, 2.0f, "falloff = %.3f");
    ImGui::SliderFloat("mod", &mod, 0.0, 5.0f, "mod = %.3f");
    ImGui::ColorEdit3("light color", (float*)&light_color);
    ImGui::End();
  DGUI_END

  shader_->set_uniform("Mod", mod);
  shader_->set_uniform("light", 0, "position", glm::vec3(light_pos.x, light_pos.y, light_pos.z));
  shader_->set_uniform("lightColor", glm::vec3(light_color.x, light_color.y, light_color.z));
  shader_->set_uniform("SphereRadius", f1);
  shader_->set_uniform("LightFalloff", f2);
  shader_->set_uniform("Texture", 0, texture1_);

  shader_->set_uniform("UseTex", usetex);

  vao_->draw();

  shader_->unuse();
}

void Cube::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}


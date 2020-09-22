#include <Mesh.hpp>

#include <MeshMaker.hpp>
#include <ResourceManager.hpp>
#include <LoadObj.hpp>

Mesh::Mesh(const char * name)
  :SceneObject(name)
{
}

Mesh::~Mesh()
{
}

bool Mesh::init()
{
  vao_ = ResourceManager::instance()->get_model("debug-camera");
  shader_ = ResourceManager::instance()->get_shader_program("Phong");

  if (!vao_ || !shader_)
    return false;

  return true;
}

void Mesh::tick()
{
}

void Mesh::draw(std::shared_ptr<CameraObject> camera)
{
  LOG_FATAL_ASSERT(vao_ && shader_, "Did not init");
  shader_->use();

  /*shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("Time", (float)glfwGetTime());
  shader_->set_uniform("Camera", camera->get_eye());*/

  static float f1 = 0.0f, f2=0.0f;
  static ImVec4 light_color(0.5f, 0.5f, 0.5f, 1.0f);
  static float mod = 2.0f;
  static bool usetex = true;
  ImVec4 light_pos(sin(glfwGetTime())*5.0f, 1.0f,cos(glfwGetTime())* 5.0f, 0.0f);

  DGUI_BEGIN;
    ImGui::Begin("Slider");
    ImGui::Checkbox("usetex", &usetex);
    ImGui::SliderFloat("distance", &f1, -1.0f, 20.0f, "distance = %.3f");
    ImGui::SliderFloat("falloff", &f2, 0.0, 1.0f, "falloff = %.3f");
    ImGui::SliderFloat("mod", &mod, 0.0, 5.0f, "mod = %.3f");
    ImGui::ColorEdit3("light color", (float*)&light_color);
    ImGui::End();
  DGUI_END;

  shader_->set_uniform("Mod", mod);
  shader_->set_uniform("light", 0, "position", glm::vec3(light_pos.x, light_pos.y, light_pos.z));
  shader_->set_uniform("lightColor", glm::vec3(light_color.x, light_color.y, light_color.z));
  shader_->set_uniform("LightFalloff", f2);

  shader_->set_uniform("UseTex", false);

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("Camera", camera->get_eye());

  vao_->draw();
  shader_->unuse();
}

void Mesh::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

void Mesh::set_vao(std::unique_ptr<VertexArray> vao)
{
  vao_ = std::move(vao);
}


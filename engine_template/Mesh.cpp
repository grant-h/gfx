#include <Mesh.hpp>

#include <MeshMaker.hpp>
#include <ResourceManager.hpp>

Mesh::Mesh(const char * name)
  :SceneObject(name)
{
}

Mesh::Mesh(const char * name, std::shared_ptr<SceneObject> parent)
  :SceneObject(name, parent)
{
}

Mesh::~Mesh()
{
}

bool Mesh::init()
{
  //vao_ = make_plane(1.0, 1.0);
  vao2_ = make_grid(50, 50, 40.0, 40.0, {0.0, 0.0, 0.0});
  vao_ = make_grid(50, 50, 40.0, 40.0, {1.0, 1.0, 1.0}, true);
  vao2_ = make_cube();//make_grid(50, 50, 40.0, 40.0, {1.0, 1.0, 1.0}, true);
  //vao2_ = make_axis(5.0);
  //
  shader_ = ResourceManager::instance()->get_shader_program("Phong");
  texture1_ = ResourceManager::instance()->get_texture("container");
  texture2_ = ResourceManager::instance()->get_texture("awesome_face");

  return true;
}

void Mesh::tick()
{
}

void Mesh::draw(std::shared_ptr<CameraObject> camera)
{
  shader_->use();

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("Time", (float)glfwGetTime());
  shader_->set_uniform("Camera", camera->get_eye());

  static float f1 = 0.0f, f2=0.0f;
  static ImVec4 light_color(0.5f, 0.5f, 0.5f, 1.0f);
  static float mod = 2.0f;
  ImVec4 light_pos(sin(glfwGetTime())*5.0f, 1.0f,cos(glfwGetTime())* 5.0f, 0.0f);

  ImGui::Begin("Slider");
  ImGui::SliderFloat("distance", &f1, -1.0f, 20.0f, "distance = %.3f");
  ImGui::SliderFloat("falloff", &f2, 0.0, 1.0f, "falloff = %.3f");
  ImGui::SliderFloat("mod", &mod, 0.0, 5.0f, "mod = %.3f");
  ImGui::ColorEdit3("light color", (float*)&light_color);
  ImGui::End();

  shader_->set_uniform("Mod", mod);
  shader_->set_uniform("light", 0, "position", glm::vec3(light_pos.x, light_pos.y, light_pos.z));
  shader_->set_uniform("lightColor", glm::vec3(light_color.x, light_color.y, light_color.z));
  shader_->set_uniform("SphereRadius", f1);
  shader_->set_uniform("LightFalloff", f2);
  shader_->set_uniform("Texture", 0, texture2_);
  shader_->set_uniform("Texture", 1, texture1_);

  shader_->set_uniform("UseTex", true);
  vao_->draw();
  vao2_->draw();

  shader_->unuse();
}

void Mesh::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}


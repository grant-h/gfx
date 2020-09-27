#include <Cube.hpp>

#include <MeshMaker.hpp>
#include <ResourceManager.hpp>
#include <LoadObj.hpp>

#include <SceneRenderer.hpp>

Cube::Cube(const char * name)
  :SceneObject(name)
{
}

Cube::~Cube()
{
}

bool Cube::init()
{
  vao_ = ResourceManager::instance()->get_model("lowpoly");
  shader_ = ResourceManager::instance()->get_shader_program("Phong");
  texture1_ = ResourceManager::instance()->get_texture("uv_debug");
  material_ = std::make_shared<BasicMaterial>();
  *material_ = { {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}};

  return true;
}

void Cube::tick()
{
}

void Cube::draw(SceneRenderer * render)
{
  render->draw_material_obj(this, vao_, shader_, material_);

  DGUI_BEGIN;
    ImGui::Begin((get_name() + " Material").c_str());
    ImGui::SliderFloat(" - MAT shiny", &material_->shininess, 1.0f, 20.0f);
    ImGui::ColorEdit3(" - MAT ambient", (float*)glm::value_ptr(material_->ambient));
    ImGui::ColorEdit3(" - MAT diffuse", (float*)glm::value_ptr(material_->diffuse));
    ImGui::ColorEdit3(" - MAT specular", (float*)glm::value_ptr(material_->specular));
    ImGui::End();
  DGUI_END;
}

void Cube::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}


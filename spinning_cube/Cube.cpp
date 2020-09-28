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
  vao_ = ResourceManager::instance()->get_model("invcube");
  shader_ = ResourceManager::instance()->get_shader_program("Phong");
  material_ = std::make_shared<BasicMaterial>();
  *material_ = { {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}};
  textures_ = std::make_shared<TextureMap>();

  textures_->set_albedo(ResourceManager::instance()->get_texture("uv_debug"));

  return true;
}

void Cube::tick()
{
}

void Cube::draw(SceneRenderer * render)
{
  //render->draw_material_obj(this, vao_, shader_, material_);
  render->draw_textured_obj(this, vao_, shader_, textures_);

  DGUI_BEGIN;
    ImGui::Begin((get_name() + " Material").c_str());
    ImGui::SliderFloat(" - MAT shiny", &material_->shininess, 1.0f, 2000.0f);
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


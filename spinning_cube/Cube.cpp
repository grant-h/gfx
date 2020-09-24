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

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 kDiffuse, kSpecular, kAmbient;
};

struct BasicMaterial {
  glm::vec3 ambient, diffuse, specular;
  float shininess;
};

static BasicMaterial material = { {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}, {0.3, 0.3, 0.8}};
static PointLight gLight = { {0.0, 0.0, 0.0}, {0.8, 0.8, 1.0},
  {0.5, 0.5, 0.5},
  {0.5, 0.5, 0.5},
  {0.5, 0.5, 0.5},
};

void Cube::tick()
{
  PointLight * light = &gLight;
  light->position = glm::vec3(sin(glfwGetTime())*2.0f, 0.5f,cos(glfwGetTime())* 2.0f);
}


void Cube::draw(std::shared_ptr<CameraObject> camera)
{
  PointLight * light = &gLight;

  shader_->use();

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("MN", get_normal_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());
  shader_->set_uniform("Camera", camera->get_eye());

  DGUI_BEGIN;
    ImGui::Begin((get_name() + " Draw").c_str());
    ImGui::ColorEdit3("light color", (float*)glm::value_ptr(light->color));
    ImGui::ColorEdit3(" - ambient", (float*)glm::value_ptr(light->kAmbient));
    ImGui::ColorEdit3(" - diffuse", (float*)glm::value_ptr(light->kDiffuse));
    ImGui::ColorEdit3(" - specular", (float*)glm::value_ptr(light->kSpecular));
    ImGui::SliderFloat(" - MAT shiny", &material.shininess, 1.0f, 20.0f);
    ImGui::ColorEdit3(" - MAT ambient", (float*)glm::value_ptr(material.ambient));
    ImGui::ColorEdit3(" - MAT diffuse", (float*)glm::value_ptr(material.diffuse));
    ImGui::ColorEdit3(" - MAT specular", (float*)glm::value_ptr(material.specular));
    ImGui::End();
  DGUI_END;

  shader_->set_uniform("material.ambient", material.ambient);
  shader_->set_uniform("material.diffuse", material.diffuse);
  shader_->set_uniform("material.specular", material.specular);
  shader_->set_uniform("material.shininess", material.shininess);

  shader_->set_uniform("pointLights", 0, "worldPos", light->position);
  shader_->set_uniform("pointLights", 0, "color", light->color);
  shader_->set_uniform("pointLights", 0, "kAmbient", light->kAmbient);
  shader_->set_uniform("pointLights", 0, "kDiffuse", light->kDiffuse);
  shader_->set_uniform("pointLights", 0, "kSpecular", light->kSpecular);

  vao_->draw();

  shader_->unuse();
}

void Cube::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}


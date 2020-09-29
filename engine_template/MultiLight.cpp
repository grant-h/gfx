#include <MultiLight.hpp>

#include <Renderer.hpp>

MultiLight::MultiLight(const char * name) 
  :SceneObject(name),
  ambient_(0.25f),
  diffuse_(0.5f),
  specular_(0.25f),
  radius_(1.0f)
{
  type_ = LT_POINT;
}

MultiLight::~MultiLight()
{
}

void MultiLight::draw(SceneRenderer * renderer)
{
  DGUI_BEGIN;
    ImGui::Begin((get_name() + " Light").c_str());
    ImGui::SliderFloat("Radius", &radius_, 0.0f, 10.0f);
    ImGui::ColorEdit3("Color", (float*)glm::value_ptr(color_));
    ImGui::SliderFloat("Ambient", (float*)&ambient_, 0.0f, 2.0f);
    ImGui::SliderFloat("Diffuse", (float*)&diffuse_, 0.0f, 2.0f);
    ImGui::SliderFloat("Specular", (float*)&specular_, 0.0f, 2.0f);
    ImGui::End();
  DGUI_END;
}

void MultiLight::make_point_light(glm::vec3 position, glm::vec3 color, float radius)
{
  type_ = LT_POINT;

  this->position(position);
  color_ = color;
  radius_ = radius;
}

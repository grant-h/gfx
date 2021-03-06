#include <MultiLight.hpp>

#include <Renderer.hpp>

MultiLight::MultiLight(const char * name) 
  :SceneObject(name),
  ambient_(0.10f),
  diffuse_(1.0f),
  specular_(0.50f),
  radius_(1.0f),
  cutoff_(12.5f),
  outer_cutoff_(14.5f)
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

    if (type_ == LT_POINT) {
      ImGui::SliderFloat("Radius", &radius_, 0.0f, 10.0f);
    } else if (type_ == LT_SPOTLIGHT) {
      ImGui::SliderFloat("Inner Cutoff", &cutoff_, 0.0f, 45.0f);
      ImGui::SliderFloat("Outer Cutoff", &outer_cutoff_, 0.0f, 45.0f);
    }

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

void MultiLight::make_directional_light(glm::vec3 direction, glm::vec3 color)
{
  type_ = LT_DIRECTIONAL;

  this->direction_ = direction;
  color_ = color;
}

void MultiLight::make_spotlight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float cutoff, float outerCutoff)
{
  type_ = LT_SPOTLIGHT;

  this->position(position);
  this->direction_ = direction;
  color_ = color;
  cutoff_ = cutoff;
  outer_cutoff_ = outerCutoff;
}

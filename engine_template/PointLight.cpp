#include <PointLight.hpp>

PointLight::PointLight(const char * name) 
  :SceneObject(name)
{
}

PointLight::~PointLight()
{
}

bool PointLight::init()
{
  return true;
}

void PointLight::draw(SceneRenderer * renderer)
{
}

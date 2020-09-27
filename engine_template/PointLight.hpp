#ifndef _POINTLIGHT_HPP
#define _POINTLIGHT_HPP

#include <glm/glm.hpp>
#include <memory>

#include <CameraObject.hpp>

class PointLight : public SceneObject {
  public:
    PointLight(const char * name);
    virtual ~PointLight();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(SceneRenderer *) override;

    glm::vec3 & get_color() { return color_; }
    glm::vec3 & get_ambient() { return ambient_; }
    glm::vec3 & get_diffuse() { return diffuse_; }
    glm::vec3 & get_specular() { return specular_; }

    void set_color(glm::vec3 & v) { color_ = v; }
    void set_ambient(glm::vec3 & v) { ambient_ = v; }
    void set_diffuse(glm::vec3 & v) { diffuse_ = v; }
    void set_specular(glm::vec3 & v) { specular_ = v; }

  private:
    glm::vec3 color_, ambient_, diffuse_, specular_;

};

#endif // _POINTLIGHT_HPP

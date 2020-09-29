#ifndef _MULTI_LIGHT_HPP
#define _MULTI_LIGHT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <SceneObject.hpp>

class MultiLight : public SceneObject {
  public:
    MultiLight(const char * name);
    virtual ~MultiLight();

    virtual bool init() override { return true; }
    virtual void tick() override {}
    virtual void draw(SceneRenderer *) override;

    enum light_type {
      LT_POINT = 0, // position, falloff radius, intensity
      LT_DIRECTIONAL, // direction, no falloff, intensity
      LT_SPOTLIGHT // position, direction, falloff, intensity
    };

    enum light_type get_type() { return type_; }
    void make_point_light(glm::vec3 position, glm::vec3 color, float radius);
    void make_directional_light(glm::vec3 direction, glm::vec3 color);
    void make_spotlight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float cutoff, float outerCutoff);

    glm::vec3 & get_color() { return color_; }
    float get_ambient() { return ambient_; }
    float get_diffuse() { return diffuse_; }
    float get_specular() { return specular_; }

    void set_color(glm::vec3 & v) { color_ = v; }
    void set_ambient(float v) { ambient_ = v; }
    void set_diffuse(float v) { diffuse_ = v; }
    void set_specular(float v) { specular_ = v; }

    void set_direction(glm::vec3 & v) { set_rotation(v); }
    glm::vec3 & get_direction() { return direction_; }

    void set_radius(float r) { radius_ = r; }
    float get_radius() { return radius_; }

    // In degrees
    void set_spotlight_cutoff(float inner, float outer) { cutoff_ = inner; outer_cutoff_ = outer; }
    float get_spotlight_cutoff() { return cutoff_; }
    float get_spotlight_outer_cutoff() { return outer_cutoff_; }
  private:
    enum light_type type_;
    glm::vec3 color_;
    float ambient_, diffuse_, specular_;

    glm::vec3 direction_; // directional, spotlight
    float radius_; // point light attenuation
    float cutoff_;
    float outer_cutoff_;
};

#endif // _MULTI_LIGHT_HPP

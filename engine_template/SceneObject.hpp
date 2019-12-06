#ifndef _SCENE_OBJECT_HPP
#define _SCENE_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <Viewport.hpp>

class SceneObject {
  public:
    SceneObject(const char * name);
    SceneObject(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~SceneObject();

    virtual bool init() = 0;
    virtual void tick() = 0;
    virtual void draw(Viewport *) = 0;

    void position(glm::vec3 & pos);
    void position(float x, float y, float z);
    glm::vec3 position();
    glm::mat4 get_model_matrix();

    std::string to_string();
  private:
    std::string object_name_;
    std::weak_ptr<SceneObject> parent_object_;

    // transforms / rotations
    glm::vec3 position_;
    glm::vec3 rotation_; // in radians
    float scale_;
};

#endif // _SCENE_OBJECT_HPP

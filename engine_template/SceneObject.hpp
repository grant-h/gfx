#ifndef _SCENE_OBJECT_HPP
#define _SCENE_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class CameraObject;
class SceneRenderer;

class SceneObject: public std::enable_shared_from_this<SceneObject> {
  public:
    SceneObject(const char * name);
    virtual ~SceneObject();

    virtual bool init() = 0;
    virtual void tick() = 0;
    virtual void draw(SceneRenderer *) = 0;

    virtual void position(glm::vec3 & pos);
    virtual void position(float x, float y, float z);
    virtual glm::vec3 position();

    virtual void set_scale(float scale) { scale_ = glm::vec3(scale); }
    virtual void set_scale(float x, float y, float z) { scale_ = glm::vec3(x, y, z); }
    virtual void set_scale(glm::vec3 scale) { scale_ = scale; }
    virtual glm::vec3 get_scale() { return scale_; }

    virtual void set_rotation(glm::vec3 & rotation) { rotation_ = rotation; }
    virtual glm::vec3 get_rotation() { return rotation_; }

    glm::mat4 get_model_matrix();
    glm::mat4 get_normal_matrix();

    std::vector<std::shared_ptr<SceneObject> >::const_iterator iter_children() {
      return child_objects_.begin();
    }

    std::vector<std::shared_ptr<SceneObject> >::const_iterator iter_children_end() {
      return child_objects_.end();
    }

    void add_child_object(std::shared_ptr<SceneObject> child);

    std::string to_string();
    std::string get_name() { return object_name_; }

  protected:
    std::weak_ptr<SceneObject> parent_object_;

  private:
    std::string object_name_;
    std::vector<std::shared_ptr<SceneObject> > child_objects_;

    // transforms / rotations
    glm::vec3 position_;
    glm::vec3 rotation_; // in radians
    glm::vec3 scale_;
};

#endif // _SCENE_OBJECT_HPP

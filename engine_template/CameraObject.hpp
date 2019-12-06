#ifndef _CAMERA_OBJECT_HPP
#define _CAMERA_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <SceneObject.hpp>

class CameraObject : public SceneObject {
  public:
    CameraObject(const char * name);
    CameraObject(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~CameraObject();

    void position(glm::vec3 & pos) override;
    void position(float x, float y, float z) override;

    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();

    void set_fov(float fov); // in degrees
    void set_near_clip(float near);
    void set_far_clip(float far);
    void set_aspect_ratio(float aspect);

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override {}
  private:
    void calculate_view();

    float fov_, near_, far_, aspect_ratio_;
    glm::vec3 camera_eye_;
    glm::mat4 view_;
    glm::mat4 projection_;
};

#endif // _CAMERA_OBJECT_HPP

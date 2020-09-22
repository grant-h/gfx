#ifndef _CAMERA_OBJECT_HPP
#define _CAMERA_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <SceneObject.hpp>

class CameraObject : public SceneObject {
  public:
    CameraObject(const char * name);
    virtual ~CameraObject();

    void position(glm::vec3 & pos) override;
    void position(float x, float y, float z) override;
    virtual glm::vec3 position() override { return SceneObject::position(); }
    glm::vec3 get_eye() { return camera_eye_; }

    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();

    void set_fov(float fov); // in degrees
    void set_near_clip(float near);
    void set_far_clip(float far);
    void set_aspect_ratio(float aspect);
    void set_yaw(float yaw)  { yaw_ = yaw; }
    void set_pitch(float pitch)  { pitch_ = pitch; }
    float get_yaw()  { return yaw_; }
    float get_pitch()  { return pitch_; }

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override;
  private:
    void calculate_view();

    bool show_debug_camera_;
    float fov_, near_, far_, aspect_ratio_;
    float yaw_, pitch_;
    glm::vec3 camera_eye_;
    glm::mat4 view_;
    glm::mat4 projection_;
};

#endif // _CAMERA_OBJECT_HPP

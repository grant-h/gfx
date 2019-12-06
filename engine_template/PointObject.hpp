#ifndef _POINT_OBJECT_HPP
#define _POINT_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <SceneObject.hpp>
#include <CameraObject.hpp>
#include <VertexArray.hpp>
#include <ShaderProgram.hpp>

class PointObject : public SceneObject {
  public:
    PointObject(const char * name);
    PointObject(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~PointObject();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override;

    void set_color(float r, float g, float b);
    void set_shader(std::shared_ptr<ShaderProgram> shader);
  private:
    VertexArray vao_;
    std::shared_ptr<ShaderProgram> shader_;
    GLuint programM_;
    GLuint programV_;
    GLuint programP_;
    glm::vec3 color_;
};

#endif // _POINT_OBJECT_HPP

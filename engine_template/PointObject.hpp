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
    virtual ~PointObject();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(SceneRenderer *) override;

    void set_color(float r, float g, float b);
    void set_shader(std::shared_ptr<ShaderProgram> shader);
  private:
    std::shared_ptr<VertexArray> vao_;
    std::shared_ptr<ShaderProgram> shader_;
    glm::vec3 color_;
};

#endif // _POINT_OBJECT_HPP

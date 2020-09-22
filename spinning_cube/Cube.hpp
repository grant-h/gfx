#ifndef _CUBE_HPP
#define _CUBE_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include <Texture.hpp>
#include <SceneObject.hpp>
#include <CameraObject.hpp>
#include <VertexArray.hpp>
#include <ShaderProgram.hpp>

class Cube : public SceneObject {
  public:
    Cube(const char * name);
    virtual ~Cube();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override;

    void set_shader(std::shared_ptr<ShaderProgram> shader);
  private:
    std::unique_ptr<VertexArray> vao_;
    std::shared_ptr<Texture> texture1_;
    std::shared_ptr<ShaderProgram> shader_;
};

#endif // _CUBE_HPP

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
#include <SceneRenderer.hpp>

class Cube : public SceneObject {
  public:
    Cube(const char * name);
    virtual ~Cube();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(SceneRenderer *) override;

    void set_shader(std::shared_ptr<ShaderProgram> shader);
  private:
    std::shared_ptr<VertexArray> vao_;
    std::shared_ptr<Texture> texture1_;
    std::shared_ptr<ShaderProgram> shader_;
    std::shared_ptr<BasicMaterial> material_;
};

#endif // _CUBE_HPP

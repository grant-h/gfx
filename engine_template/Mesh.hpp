#ifndef _MESH_HPP
#define _MESH_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include <Texture.hpp>
#include <SceneObject.hpp>
#include <CameraObject.hpp>
#include <VertexArray.hpp>
#include <ShaderProgram.hpp>

class Mesh : public SceneObject {
  public:
    Mesh(const char * name);
    Mesh(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~Mesh();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override;

    void set_shader(std::shared_ptr<ShaderProgram> shader);
  private:
    std::unique_ptr<VertexArray> vao_;
    std::unique_ptr<VertexArray> vao2_;
    std::shared_ptr<Texture> texture1_;
    std::shared_ptr<ShaderProgram> shader_;
};

#endif // _MESH_HPP

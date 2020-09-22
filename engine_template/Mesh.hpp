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
    virtual ~Mesh();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(std::shared_ptr<CameraObject> camera) override;

    void set_shader(std::shared_ptr<ShaderProgram> shader);
    void set_vao(std::unique_ptr<VertexArray> vao);
  private:
    std::shared_ptr<VertexArray> vao_;
    std::shared_ptr<ShaderProgram> shader_;
};

#endif // _MESH_HPP

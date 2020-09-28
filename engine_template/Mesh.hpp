#ifndef _MESH_HPP
#define _MESH_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include <SceneObject.hpp>
#include <Texture.hpp>
#include <VertexArray.hpp>
#include <ShaderProgram.hpp>
#include <SceneRenderer.hpp>

class Mesh : public SceneObject {
  public:
    Mesh(const char * name);
    virtual ~Mesh();

    virtual bool init() override;
    virtual void tick() override;
    virtual void draw(SceneRenderer * renderer) override;

    void set_shader(std::shared_ptr<ShaderProgram> shader);
    void set_geometry(std::shared_ptr<VertexArray> vao);
    void set_material(std::shared_ptr<BasicMaterial> mat);
    void set_textures(std::shared_ptr<TextureMap> textures);
  private:
    std::shared_ptr<VertexArray> vao_;
    std::shared_ptr<ShaderProgram> shader_;
    std::shared_ptr<BasicMaterial> material_;
    std::shared_ptr<TextureMap> textures_;
};

#endif // _MESH_HPP

#include <Mesh.hpp>

#include <SceneRenderer.hpp>
#include <MeshMaker.hpp>
#include <ResourceManager.hpp>
#include <LoadObj.hpp>

Mesh::Mesh(const char * name)
  :SceneObject(name)
{
}

Mesh::~Mesh()
{
}

bool Mesh::init()
{
  return true;
}

void Mesh::tick()
{
}

void Mesh::draw(SceneRenderer * renderer)
{
  // we need at least geometry and a shader
  if (!vao_ || !shader_)
    return;

  // prefer textured objects to material only
  if (textures_ && textures_->texture_count()) {
    renderer->draw_textured_obj(this, vao_, shader_, textures_);
  } else if (material_) {
    renderer->draw_material_obj(this, vao_, shader_, material_);
  } else {
    renderer->draw_basic_obj(this, vao_, shader_);
  }
}

void Mesh::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

void Mesh::set_geometry(std::shared_ptr<VertexArray> vao)
{
  vao_ = vao;
}

void Mesh::set_material(std::shared_ptr<BasicMaterial> mat)
{
  material_ = mat;
}

void Mesh::set_textures(std::shared_ptr<TextureMap> textures)
{
  textures_ = textures;
}

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
  vao_ = ResourceManager::instance()->get_model("debug-camera");
  shader_ = ResourceManager::instance()->get_shader_program("Phong");

  if (!vao_ || !shader_)
    return false;

  return true;
}

void Mesh::tick()
{
}

void Mesh::draw(SceneRenderer * renderer)
{
  if (!vao_ || !shader_)
    return;

  renderer->draw_basic_obj(this, vao_, shader_);
}

void Mesh::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}

void Mesh::set_vao(std::unique_ptr<VertexArray> vao)
{
  vao_ = std::move(vao);
}

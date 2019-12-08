#include <Mesh.hpp>

#include <MeshMaker.hpp>
#include <ResourceManager.hpp>

Mesh::Mesh(const char * name)
  :SceneObject(name)
{
}

Mesh::Mesh(const char * name, std::shared_ptr<SceneObject> parent)
  :SceneObject(name, parent)
{
}

Mesh::~Mesh()
{
}

bool Mesh::init()
{
  //vao_ = make_plane(1.0, 1.0);
  vao2_ = make_grid(50, 50, 20.0, 20.0, {0.0, 1.0, 0.0});
  vao_ = make_grid(50, 50, 20.0, 20.0, {1.0, 0.0, 0.0}, true);
  //vao2_ = make_axis(5.0);
  //
  shader_ = ResourceManager::instance()->get_shader_program("Simple2");

  return true;
}

void Mesh::tick()
{
}

void Mesh::draw(std::shared_ptr<CameraObject> camera)
{
  shader_->use();

  shader_->set_uniform("M", get_model_matrix());
  shader_->set_uniform("V", camera->get_view_matrix());
  shader_->set_uniform("P", camera->get_projection_matrix());

  vao_->draw();
  vao2_->draw();

  shader_->unuse();
}

void Mesh::set_shader(std::shared_ptr<ShaderProgram> shader)
{
  shader_ = shader;
}


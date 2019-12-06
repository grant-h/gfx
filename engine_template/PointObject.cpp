#include "PointObject.hpp"

#include <sstream>

#include <Log.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

PointObject::PointObject(const char * name)
  :SceneObject(name)
{
}

PointObject::PointObject(const char * name, std::shared_ptr<SceneObject> parent)
  :SceneObject(name, parent)
{
}

PointObject::~PointObject()
{
  LOG_TRACE_FUNC("delete");
}

#include <common/shader.hpp>
static std::string getResource(const char * path) {
   return std::string("data/") + std::string(path);
}
static std::string getResource(std::string path) {
   return getResource(path.c_str());
}
static std::string getShader(const char * path) {
   return getResource(std::string("shader/") + std::string(path));
}

bool PointObject::init()
{
  std::vector<Vertex> vec;
  Vertex v = {0.0, 0.0, 0.0, 1.0};
  vec.push_back(v);
  if (!vao_.create(vec))
    return false;

  programID_ = LoadShaders(getShader("StandardShading.vertexshader").c_str(), getShader("StandardShading.fragmentshader").c_str());
  programM_ = glGetUniformLocation(programID_, "M");
  programV_ = glGetUniformLocation(programID_, "V");
  programP_ = glGetUniformLocation(programID_, "P");

  if (programM_ < 0 || programV_ < 0 || programP_ < 0) {
    return false;
  }

  if (programID_ == 0) {
    return false;
  }

  return true;
}

void PointObject::tick()
{
}

void PointObject::draw(Viewport * viewport)
{
  //ScopedVertexArray sva(&vao_);

  vao_.activate();
  glUseProgram(programID_);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glUniformMatrix4fv(programM_, 1, GL_FALSE, &get_model_matrix()[0][0]);
  glUniformMatrix4fv(programV_, 1, GL_FALSE, &viewport->view[0][0]);
  glUniformMatrix4fv(programP_, 1, GL_FALSE, &viewport->projection[0][0]);

  glDrawArrays(GL_POINTS, 0, 1);

  glDisable(GL_PROGRAM_POINT_SIZE);
  glUseProgram(0);
}

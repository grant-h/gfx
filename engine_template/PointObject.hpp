#ifndef _POINT_OBJECT_HPP
#define _POINT_OBJECT_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <SceneObject.hpp>
#include <VertexArray.hpp>

class PointObject : public SceneObject {
  public:
    PointObject(const char * name);
    PointObject(const char * name, std::shared_ptr<SceneObject> parent);
    virtual ~PointObject();

    virtual bool init();
    virtual void tick();
    virtual void draw(Viewport *);
  private:
    VertexArray vao_;
    GLuint programID_;
    GLuint programM_;
    GLuint programV_;
    GLuint programP_;
    //ShaderProgram program_;
    glm::vec3 color_;
};

#endif // _POINT_OBJECT_HPP

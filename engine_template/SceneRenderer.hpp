// shared stuff
//  - lights in scene
//  - camera
#ifndef _SCENE_RENDERER_HPP
#define _SCENE_RENDERER_HPP

#include <SceneObject.hpp>
#include <Texture.hpp>
#include <ShaderProgram.hpp>
#include <VertexArray.hpp>

#include <map>
#include <memory>
#include <vector>

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 kDiffuse, kSpecular, kAmbient;
};

struct BasicMaterial {
  glm::vec3 ambient, diffuse, specular;
  float shininess;
};


class TextureMap {
  public:
    void set_albedo(std::shared_ptr<Texture> tex) { textures_["albedo"] = tex; }
    // alias for albedo
    void set_diffuse(std::shared_ptr<Texture> tex) { set_albedo(tex); }
  private:
    std::map<std::string, std::shared_ptr<Texture> > textures_;
};

struct DrawCommand {
  SceneObject * obj;
  std::shared_ptr<VertexArray> vao;
  std::shared_ptr<ShaderProgram> shader;
  std::shared_ptr<BasicMaterial> material;
  std::shared_ptr<TextureMap> texture_map;
};

class SceneRenderer {
  public:
    void draw_point(SceneObject * obj, std::shared_ptr<VertexArray> & vao,
        std::shared_ptr<ShaderProgram> & shader) {
      // TODO: point size
      commands_.push_back({obj, vao, shader, nullptr, nullptr});
    }

    void draw_basic_obj(SceneObject * obj, std::shared_ptr<VertexArray> & vao,
        std::shared_ptr<ShaderProgram> & shader) {
      commands_.push_back({obj, vao, shader, nullptr, nullptr});
    }

    void draw_material_obj(SceneObject * obj, std::shared_ptr<VertexArray> & vao,
        std::shared_ptr<ShaderProgram> & shader, std::shared_ptr<BasicMaterial> & mat) {
      commands_.push_back({obj, vao, shader, mat, nullptr});
    }

    void draw_textured_obj(SceneObject * obj, std::shared_ptr<VertexArray> & vao,
        std::shared_ptr<ShaderProgram> & shader, std::shared_ptr<TextureMap> & texture_map) {
      commands_.push_back({obj, vao, shader, nullptr, texture_map});
    }

  //private:
    std::vector<DrawCommand> commands_;
};


#endif // _SCENE_RENDERER_HPP

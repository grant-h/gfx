#include <LoadObj.hpp>

#include <glm/glm.hpp>
#include <StringUtil.hpp>
#include <Log.hpp>

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <unordered_map>

struct FaceCommand {
  glm::ivec3 elem[3];
};

namespace std {
  template<> struct hash<glm::ivec3>
  {
    // crappy hash, but comparision operator will handle the rest
    std::size_t	operator()(const glm::ivec3 & p) const noexcept
    {
      return p.x*100 + p.y*10 + p.z;
    }
  };
}

std::unique_ptr<VertexArray> load_obj(const char * filename) {
  std::vector<glm::vec3> verts;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> norms;
  std::unordered_map<glm::ivec3,size_t> uniq_verts;
  std::vector<FaceCommand> faces;

  std::vector<VertexCNT> vec_emit;

  size_t vert_idx = 0;

  auto stream = std::fstream(filename);

  if (!stream.is_open()) {
    return nullptr;
  }

  int line_no = 0;

  for (std::string line; std::getline(stream, line); ) {
    line_no++;

    line = trim(line);

    if (line.length() == 0 || line.at(0) == '#')
      continue;

    std::vector<std::string> tokens = split(line, ' ', true);

    if (tokens.size() == 0)
      continue;

    std::string command = tokens[0];
    std::vector<std::string> args = std::vector<std::string>(tokens.begin()+1, tokens.end());

    if (command == "v") {
      if (args.size() != 3) {
        LOG_ERROR("%s:%d [v]ertex command needs three parameters", filename, line_no);
        return nullptr;
      }

      try {
        verts.push_back(glm::vec3(
          std::stof(args[0]),
          std::stof(args[1]),
          std::stof(args[2])
        ));
      } catch (std::out_of_range & e) {
        LOG_ERROR("%s:%d [v]ertex command invalid floating point number", filename, line_no);
        return nullptr;
      } catch (std::invalid_argument & e) {
        LOG_ERROR("%s:%d [v]ertex command invalid floating point number", filename, line_no);
        return nullptr;
      }
    } else if (command == "vt") {
      if (args.size() != 2) {
        LOG_ERROR("%s:%d [vt]exture command needs two parameters", filename, line_no);
        return nullptr;
      }

      try {
        uvs.push_back(glm::vec2(
          std::stof(args[0]),
          std::stof(args[1])
        ));
      } catch (std::out_of_range & e) {
        LOG_ERROR("%s:%d [vt]exture command invalid floating point number", filename, line_no);
        return nullptr;
      } catch (std::invalid_argument & e) {
        LOG_ERROR("%s:%d [vt]exture command invalid floating point number", filename, line_no);
        return nullptr;
      }
    } else if (command == "vn") {
      if (args.size() != 3) {
        LOG_ERROR("%s:%d [vn]ormal command needs three parameters", filename, line_no);
        return nullptr;
      }

      try {
        norms.push_back(glm::vec3(
          std::stof(args[0]),
          std::stof(args[1]),
          std::stof(args[2])
        ));
      } catch (std::out_of_range & e) {
        LOG_ERROR("%s:%d [vn]ormal command invalid floating point number", filename, line_no);
        return nullptr;
      } catch (std::invalid_argument & e) {
        LOG_ERROR("%s:%d [vn]ormal command invalid floating point number", filename, line_no);
        return nullptr;
      }
    } else if (command == "f") {
      if (args.size() != 3) {
        LOG_ERROR("%s:%d [f]ace command needs three parameters (triangulated)", filename, line_no);
        return nullptr;
      }

      FaceCommand cmd;

      for (size_t i = 0; i < 3; i++) {
        std::vector<std::string> cmds = split(args.at(i), '/');

        if (cmds.size() != 3) {
          LOG_ERROR("%s:%d [f]ace element requires vertex, texture, and normal indexes", filename, line_no);
          return nullptr;
        }

        glm::ivec3 c;

        try {
          c.x = std::stoi(cmds[0]);
          c.y = std::stoi(cmds[1]);
          c.z = std::stoi(cmds[2]);
        } catch (std::out_of_range & e) {
          LOG_ERROR("%s:%d [f]ace element index is out of range or not an integer", filename, line_no);
          return nullptr;
        } catch (std::invalid_argument & e) {
          LOG_ERROR("%s:%d [f]ace element index is out of range or not an integer", filename, line_no);
          return nullptr;
        }

        if (c.x == 0 || c.x > (int)verts.size() || c.x < -(int)verts.size()) {
          LOG_ERROR("%s:%d [f]ace vertex index is out of range", filename, line_no);
          return nullptr;
        }

        if (c.x < 0) c.x = verts.size() + c.x;
        else c.x--;

        if (c.y == 0 || c.y > (int)uvs.size() || c.y < -(int)uvs.size()) {
          LOG_ERROR("%s:%d [f]ace texture index is out of range", filename, line_no);
          return nullptr;
        }

        if (c.y < 0) c.y = uvs.size() + c.y;
        else c.y--;

        if (c.z == 0 || c.z > (int)norms.size() || c.z < -(int)norms.size()) {
          LOG_ERROR("%s:%d [f]ace normal index is out of range", filename, line_no);
          return nullptr;
        }

        if (c.z < 0) c.z = norms.size() + c.z;
        else c.z--;

        cmd.elem[i] = c;

        // find all uniq index combinations to emit verticies for
        if (uniq_verts.find(c) == uniq_verts.end()) {
          uniq_verts[c] = vert_idx++;

          glm::vec3 pos = verts[c.x];
          glm::vec2 tex = uvs[c.y];
          glm::vec3 norm = norms[c.z];

          VertexCNT v = { {pos.x, pos.y, pos.z, 1.0}, {1.0, 1.0, 1.0, 1.0},
            {norm.x, norm.y, norm.z, 0.0}, {tex.x, tex.y} };
          vec_emit.push_back(v);
        }
      }

      faces.push_back(cmd);
    } else {
      LOG_WARN("Unhandled OBJ command '%s'", command.c_str());
    }
  }

  // emit index commands
  std::vector<uint32_t> indicies;
  indicies.reserve(faces.size()*3);

  for (auto it = faces.begin(); it != faces.end(); it++) {
    FaceCommand cmd = *it;

    indicies.push_back(uniq_verts[cmd.elem[0]]);
    indicies.push_back(uniq_verts[cmd.elem[1]]);
    indicies.push_back(uniq_verts[cmd.elem[2]]);
  }

  LOG_INFO("Loaded model %s with %d unique verticies", filename, uniq_verts.size());

  std::unique_ptr<VertexArray> vao(new VertexArray(GL_TRIANGLES));
  assert(vao->create(vec_emit, indicies));

  return vao;
}

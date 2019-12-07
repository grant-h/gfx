#ifndef _MESH_MAKER_HPP
#define _MESH_MAKER_HPP

#include <memory>
#include <glm/glm.hpp>
#include <VertexArray.hpp>

std::unique_ptr<VertexArray> make_plane(float length, float width, glm::vec3 color = {0.0, 0.0, 0.0});
std::unique_ptr<VertexArray> make_axis(float size);
std::unique_ptr<VertexArray> make_grid(int xseg, int yseg, float gwidth, float gheight, glm::vec3 color={0.0, 0.0, 0.0}, bool triangles=false);

#endif // _MESH_MAKER_HPP

#include "MeshMaker.hpp"

std::unique_ptr<VertexArray> make_plane(float length, float width, glm::vec3 color)
{
  std::vector<VertexCNT> vec
  {
    { .position={ -length, 0.0,  width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 0.0f, 1.0f } },
    { .position={ -length, 0.0, -width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 0.0f, 0.0f } },
    { .position={  length, 0.0, -width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 1.0f, 0.0f } },

    { .position={ -length, 0.0,  width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 0.0f, 1.0f } },
    { .position={  length, 0.0, -width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 1.0f, 0.0f } },
    { .position={  length, 0.0,  width, 1.0 }, .color={ color.r, color.g, color.b, 1.0 }, .normal={ 0.0, 1.0, 0.0, 0.0 }, .texture={ 1.0f, 1.0f } },
  };

  std::unique_ptr<VertexArray> vao(new VertexArray(GL_TRIANGLES));

  assert(vao->create(vec));

  return vao;
}

std::unique_ptr<VertexArray> make_axis(float size)
{
  std::vector<VertexC> vec =
  {
    { .position={ 0.0, 0.0, 0.0, 1.0 }, .color={ 1.0, 0.0, 0.0, 1.0 }},
    { .position={ size, 0.0, 0.0, 1.0 }, .color={ 1.0, 0.0, 0.0, 1.0 }},
    { .position={ 0.0, 0.0, 0.0, 1.0 }, .color={ 0.0, 1.0, 0.0, 1.0 }},
    { .position={ 0.0, size, 0.0, 1.0 }, .color={ 0.0, 1.0, 0.0, 1.0 }},
    { .position={ 0.0, 0.0, 0.0, 1.0 }, .color={ 0.0, 0.0, 1.0, 1.0 }},
    { .position={ 0.0, 0.0, size, 1.0 }, .color={ 0.0, 0.0, 1.0, 1.0 }},
  };

  std::unique_ptr<VertexArray> vao(new VertexArray(GL_LINES));

  assert(vao->create(vec));

  return vao;
}

std::unique_ptr<VertexArray> make_cube()
{
  // TODO: fix normals + UVs
  std::vector<VertexCNT> vec
  {
    // bottom of cube
    { {-1.0, -1.0, -1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, -1.0, 0.0}, {0.0, 0.0} },
    { {1.0, -1.0, -1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, -1.0, 0.0}, {1.0, 0.0} },
    { {1.0, -1.0, 1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, -1.0, 0.0}, {1.0, 1.0} },
    { {-1.0, -1.0, 1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, -1.0, 0.0}, {0.0, 1.0} },

    // top of cube
    { {-1.0, 1.0, -1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, 1.0, 0.0}, {0.0, 0.0} },
    { {1.0, 1.0, -1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, 1.0, 0.0}, {1.0, 0.0} },
    { {1.0, 1.0, 1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, 1.0, 0.0}, {1.0, 1.0} },
    { {-1.0, 1.0, 1.0, 1.0}, {0.0, 0.8, 0.8, 1.0}, {0.0, 1.0, 0.0}, {0.0, 1.0} },
  };

  std::vector<uint32_t> indicies
  {
    0, 1, 2,  2, 3, 0, // bottom
    6, 5, 4,  4, 7, 6, // top

    0, 4, 1,  4, 5, 1, // back
    2, 6, 3,  6, 7, 3, // front

    5, 2, 1,  5, 6, 2, // right
    0, 3, 7,  0, 7, 4, // left
  };

  std::unique_ptr<VertexArray> vao(new VertexArray(GL_TRIANGLES));
  assert(vao->create(vec, indicies));

  return vao;
}

std::unique_ptr<VertexArray> make_grid(int xseg, int yseg, float gwidth, float gheight, glm::vec3 color, bool triangles)
{
  /*
   * Assume we are building a grid with its center at the origin.
   * We are able to translate the grid to anywhere we please.
   *
   *     X
   *   +---> w=2, l=2,
   * Z |
   *   |  TL
   *   v  +----+----+        12 unique lines
   *      |0   |1   |2       24 indices (for GL_LINE)
   *      |    |    |
   *      +----+----+        lines = 2*3 + 3*2
   *      |3   |4   |5       lines = w*(l+1) + (w+1)*l
   *      |    |    |
   *      +----+----+  BR
   *       6    7    8
   */

  VertexCNT bottomRight = {.position={gwidth/2, 0.0, gheight/2, 1.0}, .color={color.r, color.g, color.b, 1.0}, .normal={0.0, 1.0, 0.0, 0.0}};
  VertexCNT topLeft = {.position={-gwidth/2, 0.0, -gheight/2, 1.0}, .color={color.r, color.g, color.b, 1.0}, .normal={0.0, 1.0, 0.0, 0.0}};

  // the number of squares along each axis
  int width = xseg, length = yseg;
  // the number of actual vertices per axis
  int widthV = width+1, lengthV = length+1;
  // the number of unique lines in the grid
  int numLines = width*lengthV + widthV*length;

  float dx = bottomRight.x - topLeft.x;
  float dz = bottomRight.z - topLeft.z;
  float strideX = dx/width;
  float strideZ = dz/length;

  std::vector<VertexCNT> grid;
  grid.reserve(widthV*lengthV);
  grid.resize(widthV*lengthV);

  float v = 1.0;

  // Generate vertices
  for(int l = 0; l < lengthV; l++)
  {
    float Z = topLeft.z + strideZ*l;
    // XXX: U and V are messed up here because of the input image
    float u = 0.0;

    for(int w = 0; w < widthV; w++)
    {
      // initialize the vertex memory
      grid[l*widthV + w] = topLeft;

      grid[l*widthV + w].x = topLeft.x + strideX*w;
      grid[l*widthV + w].y = 0.0;
      grid[l*widthV + w].z = Z;
      grid[l*widthV + w].w = 1.0;

      grid[l*widthV + w].u = u;
      grid[l*widthV + w].v = v;

      u += 0.999/(widthV-1);
    }

    v -= 0.999/(lengthV-1);
  }

  // Calculate vertex normals from face normals
  for(int l = 1; l < lengthV-1; l++)
  {
    for(int w = 1; w < widthV-1; w++)
    {
      VertexCNT * self = &grid[l*widthV + w];
      VertexCNT * left = &grid[l*widthV + (w-1)];
      VertexCNT * right = &grid[l*widthV + (w+1)];
      VertexCNT * top = &grid[(l-1)*widthV + w];
      VertexCNT * bottom = &grid[(l+1)*widthV + w];

      glm::vec3 s = glm::vec3(self->x, self->y, self->z);
      glm::vec3 v1 = glm::cross(glm::vec3(top->x, top->y, top->z)-s,
          glm::vec3(left->x, left->y, left->z)-s);
      glm::vec3 v2 = glm::cross(glm::vec3(bottom->x, bottom->y, bottom->z)-s,
          glm::vec3(right->x, right->y, right->z)-s);

      glm::vec3 vt = v1;//glm::normalize(v1+v2);
      //vt = glm::normalize(glm::vec3(0.0, 0.0, 1.0));

      grid[l*widthV + w].nx = vt.x;
      grid[l*widthV + w].ny = vt.y;
      grid[l*widthV + w].nz = vt.z;
      grid[l*widthV + w].nw = 0.0;
    }
  }

  // Emit drawing commands
  std::vector<uint32_t> indices;

  if (triangles) {
    for(int s = 0; s < lengthV-1; s++) {
      int base = s*widthV;

      for(int w = 0; w < widthV-1; w++) {
        indices.push_back(base+widthV+w+1);
        indices.push_back(base+w+1);
        indices.push_back(base+w);

        indices.push_back(base+w);
        indices.push_back(base+widthV+w);
        indices.push_back(base+widthV+w+1);
      }
    }
  } else {
    for(int l = 0; l < lengthV; l++)
    {
      int base = l*widthV;

      for(int w = 0; w < widthV; w++)
      {
        // horizontal
        if(w+1 != widthV)
        {
          indices.push_back(base + w);
          indices.push_back(base + w + 1);
        }

        // vertical
        if(l+1 != lengthV)
        {
          indices.push_back(base + w);
          indices.push_back((l+1)*widthV + w);
        }
      }
    }
  }

  std::unique_ptr<VertexArray> vao(new VertexArray(triangles ? GL_TRIANGLES : GL_LINES));

  assert(vao->create(grid, indices));

  return vao;
}

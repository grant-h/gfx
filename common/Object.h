#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[4];
	float Texture[2];

	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
		Normal[3] = 0.0;
	}
	void SetTexture(float u, float v) {
		Texture[0] = u;
		Texture[1] = v;
	}
};

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
	point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
	point operator -(const point& a)const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a)const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a)const {
		return point(x*a, y*a, z*a);
	}
	point operator /(const float& a)const {
		return point(x / a, y / a, z / a);
	}

	void toArray(float * arr)
	{
		arr[0] = x;
		arr[1] = y;
		arr[2] = z;
	}
};

struct SurfacePoint {
	float x, y, z;
	float u, v;

	SurfacePoint(const float x = 0, const float y = 0, const float z = 0, const float u = 0, const float v = 0) : x(x), y(y), z(z), u(u), v(v) {};
	SurfacePoint(float *coords, float *tex)
	  : x(coords[0]), y(coords[1]), z(coords[2]),
	    u(tex[0]), v(tex[1]) {};
	SurfacePoint operator -(const SurfacePoint& a)const {
		return SurfacePoint(x - a.x, y - a.y, z - a.z,
		    u - a.u, v - a.v);
	}
	SurfacePoint operator +(const SurfacePoint& a)const {
		return SurfacePoint(x + a.x, y + a.y, z + a.z,
		    u + a.u, v + a.v);
	}
	SurfacePoint operator *(const float& a)const {
		return SurfacePoint(x*a, y*a, z*a,
		    u * a, v * a);
	}
	SurfacePoint operator /(const float& a)const {
		return SurfacePoint(x / a, y / a, z / a,
		    u / a, v / a);
	}

	void toArray(float * coord, float * tex)
	{
		coord[0] = x;
		coord[1] = y;
		coord[2] = z;

		tex[0] = u;
		tex[1] = v;
	}
};

class Object
{
  public:
    Object(const char * path, unsigned int id, unsigned int type, unsigned int shader, Object * parent)
    {
      m_path = path;
      m_parent = parent;
      m_type = type;
      m_shader = shader;
      texture = 0;
      scale = 1.0;

      this->id = id;
      visible = true;
    }

    const char * getPath() { return m_path; }
    unsigned int getType() { return m_type; }
    unsigned int getShader() { return m_shader; }

    glm::mat4 getModelMatrix() {
      using namespace glm;
      glm::mat4 m = glm::mat4(1.0);

      glm::quat q = glm::quat(rotation);
      glm::mat4 rot = mat4_cast(q);
      glm::mat4 trans = glm::translate(m, position);
      glm::mat4 sca = glm::scale(m, glm::vec3(scale));

      m = trans * rot * sca * m;

      if(m_parent) {
        return m_parent->getModelMatrix() * m;
      } else {
        return m;
      }
    }

  private:
    Object * m_parent;
    const char * m_path;
    unsigned int m_type;
    unsigned int m_shader;

  public:
    // material
    glm::vec4 color;

    // transforms / rotations
    glm::vec3 position;
    glm::vec3 rotation; // in radians
    float scale;
    glm::mat4 world;

    // meta
    unsigned int id;
    unsigned int texture;
    bool visible;
};

#endif

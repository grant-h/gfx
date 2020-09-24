#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 texcoord;

out vec4 vs_color;
out vec3 vs_frag;
out vec3 vs_normal;
out vec2 vs_texcoord;
out float vs_height;

uniform mat4 M;
uniform mat4 MN;
uniform mat4 V;
uniform mat4 P;

void main()
{
  vs_color = color;
  vs_texcoord = texcoord;

  // Note that the normal is a VECTOR. That means this multiplication
  // by M will ONLY rotate or scale the vector. Translation would change
  // the properties of the lighting. Multiply by the normal matrix to handle
  // non-uniform scaling
  vs_normal = (MN * normal).xyz; // world space

  // These MUST be in world space for interpolation to work correctly
  vs_frag = (M * vertex).xyz; // world space

  // Emit the vertex position in perspective space
  gl_Position = P * V * M * vertex;
}

#version 330 core

// We need to transform the verticies in to the right places using the provided matrices
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
uniform mat4 V;
uniform mat4 P;
uniform float SphereRadius;
uniform sampler2D Texture[2];
uniform float Time;

float sinc(float x)
{
    return sin(x) / (x*0.2 + 1.0);
}

void main()
{
  vs_color = color;
  vs_texcoord = texcoord;

  vec4 vs_mod = vec4(vertex);

  // Note that the normal is a VECTOR. That means this multiplication
  // by M will ONLY rotate or scale the vector. Translation would change
  // the properties of the lighting
  vs_normal = (M * normal).xyz; // world space

  // These MUST be in world space for interpolation to work correctly
  vs_frag = (M * vertex).xyz; // world space

  // Emit the vertex position in perspective space
  gl_Position = P * V * M * vertex;
}

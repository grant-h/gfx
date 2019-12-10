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

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float SphereRadius;

float sinc(float x)
{
    return sin(x) / (x*0.2 + 1.0);
}

void main()
{
  gl_PointSize = 8.0;

  vs_color = color;
  vs_texcoord = texcoord;

  // These MUST be in world space for interpolation to work correctly
  vs_frag = (M * vertex).xyz; // world space

  float sphere_dist = length(vs_frag - vec3(0.0, 1.0, 0.0)) - SphereRadius;
  //float strength = sinc(SphereRadius*3.1415/20.0)*2.0;
  float disp = cos(clamp(sphere_dist*0.5, -1.0, 1.0)*3.1459/1.0);

  vec4 vs_mod = vec4(vertex.x, vertex.y+disp*1.0, vertex.z, vertex.w);

  // Note that the normal is a VECTOR. That means this multiplication
  // by M will ONLY rotate or scale the vector. Translation would change
  // the properties of the lighting
  vs_normal = (M * normal).xyz; // world space

  // Emit the vertex position in perspective space
  //gl_Position = P * V * M * (vertex + normal*Mod*1.5);
  gl_Position = P * V * M * vs_mod;
}

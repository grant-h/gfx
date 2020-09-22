#version 330 core

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec4 in_norm;

out VS_OUT {
    vec3 normal;
} vs_out;

out vec4 vs_vertexColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
  gl_Position = P * V * M * vec4(in_position.xyz, 1.0);
  mat3 normalMatrix = mat3(transpose(inverse(V * M)));
  vs_out.normal = normalize(vec3(P * vec4(normalMatrix * in_norm.xyz, 0.0)));
}

#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];


uniform float gs_debug_line_length;

void GenerateLine(int index)
{
  gl_Position = gl_in[index].gl_Position;
  EmitVertex();
  gl_Position = gl_in[index].gl_Position +
    vec4(gs_in[index].normal, 0.0) * gs_debug_line_length;
  EmitVertex();
  EndPrimitive();
}

void main()
{
  GenerateLine(0);
  GenerateLine(1);
  gl_Position = gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = gl_in[1].gl_Position;
  EmitVertex();
  EndPrimitive();
}

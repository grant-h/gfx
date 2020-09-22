#version 330 core
out vec4 fs_frag_color;

uniform vec3 fs_debug_color;

void main()
{
  fs_frag_color = vec4(fs_debug_color, 1.0);
}

#version 330 core

layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;

// Output data ; will be interpolated for each fragment.
out vec4 vs_vertexColor;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform vec3 camera;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = P * V * M * vertexPosition_modelspace;
	vec4 world_pos = M * vertexPosition_modelspace;
        float max_dist = 20.0;
        float dist = clamp(distance(world_pos, vec4(camera, 0.0)), 0.0, max_dist);
        float rat = 1.0 - dist/max_dist;
	gl_PointSize = max_dist - dist;
	//gl_PointSize = 20.0;
	
	// UV of the vertex. No special space for this one.
	vs_vertexColor = vertexColor;
}


#version 330 core

layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;

// Output data ; will be interpolated for each fragment.
out vec4 vs_vertexColor;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = P * V * M * vertexPosition_modelspace;
	vs_vertexColor = vertexColor;
}


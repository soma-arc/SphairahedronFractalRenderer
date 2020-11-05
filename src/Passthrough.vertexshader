#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 a_vertex;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position = vec4(a_vertex, 0, 1);
	UV = a_vertex.xy * 0.5 + 0.5;
}


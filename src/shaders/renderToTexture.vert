#version 300 es

layout(location = 0) in vec2 a_vertex;
out vec2 v_texCoord;

void main() {
    v_texCoord = a_vertex.xy * 0.5 + 0.5;
    gl_Position = vec4(a_vertex, 0., 1.0);
}

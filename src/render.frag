#version 300 es
precision highp float;

out vec4 outColor;

uniform vec2 u_resolution;

void main() {
    outColor = vec4(gl_FragCoord.x / u_resolution.x,
                    gl_FragCoord.y / u_resolution.y,
                    0, 1);
}

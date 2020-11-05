#version 330 core

in vec2 UV;
out vec4 outColor;

uniform sampler2D u_renderedTexture;

const float DISPLAY_GAMMA_COEFF = 1. / 2.2;
vec4 gammaCorrect(vec4 rgba) {
    return vec4((min(pow(rgba.r, DISPLAY_GAMMA_COEFF), 1.)),
                (min(pow(rgba.g, DISPLAY_GAMMA_COEFF), 1.)),
                (min(pow(rgba.b, DISPLAY_GAMMA_COEFF), 1.)),
                rgba.a);
}

void main() {
    outColor = gammaCorrect(textureLod(u_renderedTexture,
                                       UV, 0.0));
}

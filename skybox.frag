#version 140
in vec2 TexCoord;
out vec4 outputColor;
uniform sampler2D texSampler;

void main() {
    outputColor = texture(texSampler, TexCoord);
}
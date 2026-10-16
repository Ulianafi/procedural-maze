#version 140
in vec3 position;
in vec2 texCoord;

uniform mat4 PVMmatrix;
out vec2 TexCoord;

void main() {
    gl_Position = PVMmatrix * vec4(position, 1.0);
    TexCoord = texCoord;
}
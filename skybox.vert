#version 140
uniform mat4 PVMmatrix;
in vec3 position;
in vec2 texCoord;
out vec2 TexCoord;

void main() {
    gl_Position = PVMmatrix * vec4(position, 1.0);
    TexCoord = texCoord;
}
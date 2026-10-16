#version 140

uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
    gl_Position = PVMmatrix * vec4(position, 1.0);

    FragPos = vec3(modelMatrix * vec4(position, 1.0));

    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    
    TexCoord = texCoord;
}
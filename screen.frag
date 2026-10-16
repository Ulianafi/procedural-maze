#version 140
in vec2 TexCoord;
out vec4 outputColor;

uniform sampler2D texSampler;
uniform float time;
uniform bool isScreenOn;

const ivec2 pattern = ivec2(2, 2);     
const float frameDuration = 0.1;    

void main() {

    vec2 rotatedUV = vec2(TexCoord.y, 1.0 - TexCoord.x);
    
    vec2 uvScale = vec2(4.8, 4.8); 
    

    vec2 uvOffset = vec2(0.48, -0.62); 


    vec2 fixedUV = rotatedUV * uvScale + uvOffset;


    fixedUV = fract(fixedUV); 

    int totalFrames = pattern.x * pattern.y;
    int frame = int(time / frameDuration) % totalFrames;

    vec2 frameSize = vec2(1.0) / vec2(pattern);
    vec2 localUV = fixedUV / vec2(pattern);
    
    int col = frame % pattern.x;
    int row = (pattern.y - 1) - (frame / pattern.x);
    
    vec2 offset = vec2(col, row) * frameSize;
    vec2 finalUV = localUV + offset;

    outputColor = texture(texSampler, finalUV);
    if (!isScreenOn) {
        outputColor = vec4(0.01, 0.01, 0.01, 1.0); 
    }
}
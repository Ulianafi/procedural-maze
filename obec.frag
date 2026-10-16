#version 140

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 outputColor;

uniform vec3 viewPos; 

uniform vec3 objectColor;
uniform bool hasTexture;
uniform sampler2D texSampler;

uniform float time;

uniform bool hasEmissive;
uniform sampler2D emissiveSampler;

uniform bool hasDirt;
uniform sampler2D dirtSampler;

uniform bool isFlashlightOn;
uniform vec3 spotDir;
uniform float spotCutOff;
uniform float spotOuterCutOff;

#define MAX_POINT_LIGHTS 20
uniform int numPointLights;
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform float pointLightIntensities[MAX_POINT_LIGHTS];


uniform float fogDensity;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec3 sunDir = vec3(-0.2, -1.0, -0.3);
    vec3 sunColor = vec3(0.15, 0.15, 0.2);     
    vec3 spotColor = vec3(1.0, 0.95, 0.8);        
    vec3 pointLightColor = vec3(0.9, 0.85, 0.7);  
    vec3 fogColor = vec3(0.85, 0.82, 0.65);

    vec4 baseColor = hasTexture ? texture(texSampler, TexCoord) : vec4(objectColor, 1.0);
    
    //discard transparent pixels (leaves on a plant)
    if (hasTexture && baseColor.a < 0.1) {
        discard;
    }

    if (hasDirt) {
      
        vec2 worldPos = vec2(FragPos.x + FragPos.z, FragPos.y);
        vec2 dirtUV = worldPos * 0.4; 

        //animate dirt sliding downwards over time
        vec2 movingDirtUV = vec2(dirtUV.x, dirtUV.y + time * 0.01);
        vec4 dirtColor = texture(dirtSampler, movingDirtUV);

        float wave1 = sin(worldPos.x * 1.2 + FragPos.z);
        float wave2 = sin(worldPos.y * 1.7);
        float organicNoise = (wave1 + wave2) * 0.5 + 0.5; 
        

        float patchMask = smoothstep(0.8, 0.7, organicNoise);
        
        float brightness = (dirtColor.r + dirtColor.g + dirtColor.b) / 3.0;
        float fakeAlpha = smoothstep(0.8, 0.3, brightness); 
        
        float finalAlpha = patchMask * fakeAlpha;
        finalAlpha *= 0.4; 

        baseColor.rgb = mix(baseColor.rgb, dirtColor.rgb, finalAlpha);
        
    }

    vec3 norm = normalize(Normal);
    vec3 totalLighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    //sun
    vec3 lightDirSun = normalize(-sunDir);
    float diffSun = max(dot(norm, lightDirSun), 0.0);
    totalLighting += (0.05 * sunColor) + (diffSun * sunColor);

    //flashlight from viewPos
    if (isFlashlightOn) {
        vec3 lightDirSpot = normalize(viewPos - FragPos);

        //calculate soft edges for the flashlight cone
        float theta = dot(lightDirSpot, normalize(-spotDir));
        float epsilon = spotCutOff - spotOuterCutOff;
        float intensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);
        
        float diffSpot = max(dot(norm, lightDirSpot), 0.0);
        //calculate light falloff over distance
        float distance = length(viewPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.0 * distance + 0.0075 * (distance * distance));

        float specularStrength = 0.5; 
        float shininess = 32.0;      
        vec3 reflectDirSpot = reflect(-lightDirSpot, norm);  
        float specSpot = pow(max(dot(viewDir, reflectDirSpot), 0.0), shininess);
        vec3 specularSpot = specularStrength * specSpot * spotColor;
        
        //phong
        totalLighting += (diffSpot * spotColor * intensity * attenuation);
    }

    //lamps
    vec3 dimLightColor = pointLightColor * 0.50;
    for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLightPositions[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        float distance = length(pointLightPositions[i] - FragPos);
    
        float attenuation = 1.0 / (1.0 + 0.5 * distance + 1.0 * (distance * distance));
    
        totalLighting += (diff * dimLightColor * pointLightIntensities[i] * attenuation);
    }
    vec3 finalColor = baseColor.rgb * totalLighting;

    //fog
    float camDistance = length(viewPos - FragPos);
    float fogFactor = exp(-pow((camDistance * fogDensity), 1.5));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    finalColor = mix(fogColor, finalColor, fogFactor);

    if (hasEmissive) {
        vec3 emissionColor = texture(emissiveSampler, TexCoord).rgb;
        finalColor += (emissionColor * 2.0); 
    }

    outputColor = vec4(finalColor, baseColor.a);
}
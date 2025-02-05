#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNorm;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

vec3 lightDir = vec3(-1.0, 1.0, -1.0);
float ambientStrength = 0.6;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    // Calculate diffuse lighting
    vec3 normalizedLightDir = normalize(lightDir);
    vec3 normalizedFragNorm = normalize(fragNorm);
    float diffuseIntensity = max(dot(normalizedLightDir, normalizedFragNorm), 0.0);

    // Combine ambient and diffuse lighting
    vec3 finalLighting = ambientStrength * colDiffuse.rgb + diffuseIntensity * colDiffuse.rgb;

    finalColor = texelColor * vec4(finalLighting, 1.0);
}


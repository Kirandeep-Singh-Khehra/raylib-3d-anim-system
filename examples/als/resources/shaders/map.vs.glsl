#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNorm;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNorm;

uniform mat4 matModel;
uniform mat4 mvp;

void main()
{
    fragTexCoord = vertexTexCoord * 30;
    fragColor = vertexColor;
    fragNorm = (matModel * vec4(vertexNorm, 0.0f)).xyz;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}


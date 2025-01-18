#version 330

#define MAX_BONE_NUM 128

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal;
in vec4 vertexBoneIds;
in vec4 vertexBoneWeights;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform mat4 boneMatrices[MAX_BONE_NUM];

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main()
{
    int boneIndex0 = int(vertexBoneIds.x);
    int boneIndex1 = int(vertexBoneIds.y);
    int boneIndex2 = int(vertexBoneIds.z);
    int boneIndex3 = int(vertexBoneIds.w);
    
    vec4 skinnedPosition =
        vertexBoneWeights.x*(boneMatrices[boneIndex0]*vec4(vertexPosition, 1.0)) +
        vertexBoneWeights.y*(boneMatrices[boneIndex1]*vec4(vertexPosition, 1.0)) + 
        vertexBoneWeights.z*(boneMatrices[boneIndex2]*vec4(vertexPosition, 1.0)) + 
        vertexBoneWeights.w*(boneMatrices[boneIndex3]*vec4(vertexPosition, 1.0));
    
    vec3 skinnedNormal = vec3(0.0);
    skinnedNormal += vertexBoneWeights.x * mat3(transpose(inverse(boneMatrices[boneIndex0]))) * vertexNormal;
    skinnedNormal += vertexBoneWeights.y * mat3(transpose(inverse(boneMatrices[boneIndex1]))) * vertexNormal;
    skinnedNormal += vertexBoneWeights.z * mat3(transpose(inverse(boneMatrices[boneIndex2]))) * vertexNormal;
    skinnedNormal += vertexBoneWeights.w * mat3(transpose(inverse(boneMatrices[boneIndex3]))) * vertexNormal;
    skinnedNormal = normalize(skinnedNormal);

    // Send vertex attributes to fragment shader
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = /*skinnedNormal; //*/normalize(vec3(matNormal*vec4(skinnedNormal, 1.0)));

    gl_Position = mvp*skinnedPosition;
}

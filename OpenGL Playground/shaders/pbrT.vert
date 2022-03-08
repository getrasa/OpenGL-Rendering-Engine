#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs2;
layout (location = 7) in ivec4 BoneIDs3;
layout (location = 8) in vec4 Weights;
layout (location = 9) in vec4 Weights2;
layout (location = 10) in vec4 Weights3;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 250;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = mat4(0.000001);
    BoneTransform += gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];
    BoneTransform += gBones[BoneIDs2[0]] * Weights2[0];
    BoneTransform += gBones[BoneIDs2[1]] * Weights2[1];
    BoneTransform += gBones[BoneIDs2[2]] * Weights2[2];
    BoneTransform += gBones[BoneIDs2[3]] * Weights2[3];
    BoneTransform += gBones[BoneIDs3[0]] * Weights3[0];
    BoneTransform += gBones[BoneIDs3[1]] * Weights3[1];
    BoneTransform += gBones[BoneIDs3[2]] * Weights3[2];
    BoneTransform += gBones[BoneIDs3[3]] * Weights3[3];

    TexCoords = aTexCoords;
    WorldPos = vec3(BoneTransform * vec4(aPos, 1.0));
    //vec4 NormalL = BoneTransform * vec4(aNormal, 1.0);
    //Normal = aNormal;
    Normal = vec3(model * BoneTransform * vec4(aNormal, 0.0));
    Tangent = vec3(model * BoneTransform * vec4(aTangent, 0.0));
    Bitangent = vec3(model * BoneTransform * vec4(aBitangent, 0.0));

    gl_Position =  projection * view * model * BoneTransform * vec4(aPos, 1.0);
}

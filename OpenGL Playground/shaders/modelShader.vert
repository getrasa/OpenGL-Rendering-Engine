#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs2;
layout (location = 7) in vec4 Weights;
layout (location = 8) in vec4 Weights2;

//out vec4 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 250;
uniform mat4 gBones[MAX_BONES];



void main()
{
    //float some = BoneIds[0];
    //mat4 BoneTransform = mat4(1.0);
    mat4 BoneTransform = mat4(0.0);
    BoneTransform += gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];
    BoneTransform += gBones[BoneIDs2[0]] * Weights2[0];
    BoneTransform += gBones[BoneIDs2[1]] * Weights2[1];
    BoneTransform += gBones[BoneIDs2[2]] * Weights2[2];
    BoneTransform += gBones[BoneIDs2[3]] * Weights2[3];

    TexCoords = aTexCoords;
    // Normal = projection * view * BoneTransform * vec4(aNormal, 0.0);
    gl_Position = projection * view * model * BoneTransform * vec4(aPos, 1.0);  
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    
}
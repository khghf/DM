#version 450
#include "Common.glsl"
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;


layout(SET(0)binding=0) uniform GlobalUniformData
{
    mat4 View;
    mat4 Proj;
}globalData; 

struct InstanceParameters 
{
    mat4 Model;
    vec4 Color;
};

layout(std430,SET(1)binding = 0) readonly buffer InstanceData
{
   InstanceParameters Parameters[];
}instanceData;


layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoord;
void main() 
{
    gl_Position     = globalData.Proj * globalData.View * instanceData.Parameters[gl_InstanceIndex].Model * vec4(inPos, 1.0);
    outColor        = instanceData.Parameters[gl_InstanceIndex].Color;
    outTexCoord     = inTexCoord;
}
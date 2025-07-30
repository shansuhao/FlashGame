#include "RootSignature.hlsl"
#include "Pipeline.hlsli"

float4 color : register(b0);
cbuffer DefaultVertexCB : register(b1)
{
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
    float4x4 ModelMatrix;
    float4x4 IT_ModelMatrix;
    float4x4 ReservedMemory[1020];
}

[RootSignature(ROOTSIG)]
VSOut main(VertexData inVertexData)
{
    VSOut vo;
    float4 positionWS = mul(ModelMatrix, inVertexData.position);
    float4 positionVS = mul(ViewMatrix, positionWS);
    vo.position = mul(ProjectionMatrix, positionVS);
    vo.color = inVertexData.texcoord + color;
    return vo;
}
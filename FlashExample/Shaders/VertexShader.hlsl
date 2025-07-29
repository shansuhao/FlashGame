#include "RootSignature.hlsl"
#include "Pipeline.hlsli"

float4 color : register(b0);

[RootSignature(ROOTSIG)]
VSOut main(VertexData inVertexData)
{
    VSOut vo;
    vo.position = inVertexData.position;
    vo.color = inVertexData.texcoord + color;
    return vo;
}
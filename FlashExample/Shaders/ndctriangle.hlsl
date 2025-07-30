struct VertexData
{
    float4 position : POSITION;
    float4 texcoord : TEXCOORD0;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
};

struct VSOut
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

cbuffer globalConstants : register(b0)
{
    float color;
}

cbuffer DefaultVertexCB : register(b1)
{
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
    float4x4 ModelMatrix;
    float4x4 IT_ModelMatrix;
    float4x4 ReservedMemory[1020];
}

VSOut MainVS(VertexData inVertexData)
{
    VSOut vo;
    float4 positionWS = mul(ModelMatrix, inVertexData.position);
    float4 positionVS = mul(ViewMatrix, positionWS);
    vo.position = mul(ProjectionMatrix, positionVS);
    vo.color = inVertexData.texcoord + color;
    return vo;
}

float4 MainPS(VSOut inPsInput) : SV_Target
{
    return inPsInput.color;
}
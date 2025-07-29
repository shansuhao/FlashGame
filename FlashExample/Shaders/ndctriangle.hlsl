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

VSOut MainVS(VertexData inVertexData)
{
    VSOut vo;
    vo.position = inVertexData.position;
    vo.color = inVertexData.texcoord + color;
    return vo;
}

float4 MainPS(VSOut inPsInput) : SV_Target
{
    return inPsInput.color;
}
struct VertexData
{
    float4 position : POSITIONT;
    float4 texcoord : TEXCOORD0;
    float4 normal : NORMAL;
};

struct VSOut
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

VSOut MainVS(VertexData inVertexData)
{
    VSOut vo;
    vo.position = inVertexData.position;
    vo.color = inVertexData.texcoord;
    return vo;
}

float4 MainPS(VSOut inPsInput) : SV_Target
{
    return inPsInput.color;
}
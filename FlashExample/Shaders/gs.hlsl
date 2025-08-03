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
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD0;
    float4 positionWS : TEXCOORD1;
};

static const float PI = 3.141592f;
cbuffer globalConstants : register(b0)
{
    float color;
};

Texture2D tex[2] : register(t0);
SamplerState samplerState : register(s0);

struct MaterialData
{
    float r;
};
StructuredBuffer<MaterialData> materialData : register(t0, space1);

cbuffer DefaultVertexCB : register(b1)
{
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
    float4x4 ModelMatrix;
    float4x4 IT_ModelMatrix;
    float4x4 ReservedMemory[1020];
};

VSOut MainVS(VertexData inVertexData)
{
    VSOut vo;
    vo.normal = mul(IT_ModelMatrix, inVertexData.normal);
    //float3 positionMS = inVertexData.position.xyz + vo.normal.xyz * sin(color.x);
    float4 positionWS = mul(ModelMatrix, inVertexData.position);
    // float4 positionVS = mul(ViewMatrix, positionWS);
    vo.position = float4(positionWS.xyz + vo.normal.xyz * sin(color.x)* 0.2f, 1.0f); //mul(ProjectionMatrix, positionVS);
    vo.positionWS = positionWS;
    vo.texcoord = inVertexData.texcoord;
    return vo;
}

[maxvertexcount(4)]
void MainGS(point VSOut inPoint[1], uint inPrimitiveID : SV_PrimitiveID, inout TriangleStream<VSOut> outTriangleStream)
{
    VSOut vo;
    float3 positionWS = inPoint[0].position.xyz;
    float3 N = normalize(inPoint[0].normal.xyz);
    vo.normal = float4(N, 0.0f);
    float3 helperVec = abs(N.y) > 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
    float3 tangent = normalize(cross(N, helperVec));
    float3 bitangent = normalize(cross(tangent, N));
    float scale = materialData[inPrimitiveID].r;
    
    vo.positionWS = float4(positionWS.xyz, 1.0f);
    
    float3 p0WS = positionWS - (bitangent * 0.5f - tangent * 0.5f) * scale; //left bottom
    float4 p0VS = mul(ViewMatrix, float4(p0WS, 1.0f));
    vo.position = mul(ProjectionMatrix, p0VS);
    vo.texcoord = float4(0.0f, 1.0f, 0.0f, 0.0f);
    outTriangleStream.Append(vo);
    
    float3 p1WS = positionWS - (bitangent * 0.5f + tangent * 0.5f) * scale; //right bottom
    float4 p1VS = mul(ViewMatrix, float4(p1WS, 1.0f));
    vo.position = mul(ProjectionMatrix, p1VS);
    vo.texcoord = float4(1.0f, 1.0f, 0.0f, 0.0f);
    outTriangleStream.Append(vo);

    float3 p2WS = positionWS + (bitangent * 0.5f + tangent * 0.5f) * scale; //left top
    float4 p2VS = mul(ViewMatrix, float4(p2WS, 1.0f));
    vo.position = mul(ProjectionMatrix, p2VS);
    vo.texcoord = float4(0.0f, 0.0f, 0.0f, 0.0f);
    outTriangleStream.Append(vo);

    float3 p3WS = positionWS + (bitangent * 0.5f - tangent * 0.5f) * scale; //right top
    float4 p3VS = mul(ViewMatrix, float4(p3WS, 1.0f));
    vo.position = mul(ProjectionMatrix, p3VS);
    vo.texcoord = float4(1.0f, 0.0f, 0.0f, 0.0f);
    outTriangleStream.Append(vo);
}

float4 MainPS(VSOut inPsInput) : SV_Target
{
    float3 N = normalize(inPsInput.normal.xyz);
    float3 bottomColor = float3(0.1f, 0.4f, 0.6f);
    float3 topColor = float3(0.7f, 0.7f, 0.7f);
    float theta = asin(N.y); //-PI/2 ~ PI/2
    theta /= PI; //-0.5~0.5
    theta += 0.5f; //0.0~1.0
    float ambientColorIntensity = 1.0;
    float3 ambientColor = lerp(bottomColor, topColor, theta) * ambientColorIntensity;

    float4 colorFromTexture = tex[0].Sample(samplerState, inPsInput.texcoord.xy);
    float4 particleColor = tex[1].Sample(samplerState, inPsInput.texcoord.xy);
    float3 surfaceColor = colorFromTexture.rgb; //mul(colorFromTexture.rgb, particleColor.rgb);
    return float4(surfaceColor, colorFromTexture.a);
}
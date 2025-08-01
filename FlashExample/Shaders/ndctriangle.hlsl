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
    vo.normal = mul(IT_ModelMatrix, inVertexData.normal);
    float3 positionMS = inVertexData.position.xyz + vo.normal * sin(color.x);
    float4 positionWS = mul(ModelMatrix, float4(positionMS, 1.0f));
    float4 positionVS = mul(ViewMatrix, positionWS);
    vo.position = mul(ProjectionMatrix, positionVS);
    vo.positionWS = positionWS;
    vo.texcoord = inVertexData.texcoord;
    return vo;
}

float4 MainPS(VSOut inPsInput) : SV_Target
{
    float3 N = normalize(inPsInput.normal.xyz);
    float3 bottomColor = float3(0.1f, 0.4f, 0.6f);
    float3 topColor = float3(0.7f, 0.7f, 0.7f);
    float theta = asin(N.y); //-PI/2 ~ PI/2
    theta /= PI; //-0.5~0.5
    theta += 0.5f; //0.0~1.0
    float ambientColorIntensity = 0.6;
    float3 ambientColor = lerp(bottomColor, topColor, theta) * ambientColorIntensity;
    float3 L = normalize(float3(1.0f,1.0f,-1.0f));
    float diffuseIntensity = max(0.0f, dot(N, L));
    float3 diffuserLightColor = float3(0.1f, 0.4f, 0.6f);
    float3 diffuseColor = diffuserLightColor * diffuseIntensity;
    
    float3 specularColor = float3(0.0, 0.0, 0.0);
    if (diffuseIntensity > 0.0f)
    {
        float3 cameraPositionWS = float3(0.0f, 0.0f, 0.0f);
        float3 V = normalize(cameraPositionWS - inPsInput.positionWS.xyz);
        float3 R = normalize(reflect(-L, N));
        float specularIntensity = pow(max(0.0f, dot(V, R)), 128.0f);
        specularColor = float3(1.0f, 1.0f, 1.0f) * specularIntensity;
    }
    float3 surfaceColor = ambientColor + diffuseColor + specularColor;
    return float4(surfaceColor, 1.0f);
}
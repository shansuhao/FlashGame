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
    //float3 positionMS = inVertexData.position.xyz + vo.normal.xyz * sin(color.x);
    float4 positionWS = mul(ModelMatrix, inVertexData.position);
    // float4 positionVS = mul(ViewMatrix, positionWS);
    vo.position = float4(positionWS.xyz + vo.normal.xyz * sin(color.x)* 0.2f, 1.0f); //mul(ProjectionMatrix, positionVS);
    vo.positionWS = positionWS;
    vo.texcoord = inVertexData.texcoord;
    return vo;
}

[maxvertexcount(4)]
void MainGS(point VSOut inPoint[1], uint inPrimitiveId:SV_PrimitiveID, inout TriangleStream<VSOut> outTriangleStream)
{
    float3 positionWS = inPoint[0].position.xyz;
    
    float3 N = normalize(inPoint[0].normal.xyz);
    float3 helperVec = abs(N.y) > 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
    float3 tangent = normalize(cross(N, helperVec));
    float3 bitangent = normalize(cross(tangent, N));
    float scale = 0.1f;
    
    VSOut vo;
    vo.positionWS = inPoint[0].positionWS;
    vo.texcoord = inPoint[0].texcoord;
    vo.normal = float4(N, 1.0f);
    
    float3 p0WS = positionWS.xyz + (bitangent * (-0.5f) + tangent * (-0.5f)) * 0.1f; //left bottom
    float4 p0VS = mul(ViewMatrix, float4(p0WS.xyz, 1.0f));
    vo.position = mul(ProjectionMatrix, p0VS);
    outTriangleStream.Append(vo);
    
    float3 p1WS = positionWS.xyz + (bitangent * (0.5f) + tangent * (-0.5f)) * 0.1f;
    float4 p1VS = mul(ViewMatrix, float4(p1WS.xyz, 1.0f));
    vo.position = mul(ProjectionMatrix, p1VS);
    outTriangleStream.Append(vo);
    
    float3 p2WS = positionWS.xyz + (bitangent * (-0.5f) + tangent * (0.5f)) * 0.1f;
    float4 p2VS = mul(ViewMatrix, float4(p2WS.xyz, 1.0f));
    vo.position = mul(ProjectionMatrix, p2VS);
    outTriangleStream.Append(vo);
    
    float3 p3WS = positionWS.xyz + (bitangent * (0.5f) + tangent * (0.5f)) * 0.1f;
    float4 p3VS = mul(ViewMatrix, float4(p3WS.xyz, 1.0f));
    vo.position = mul(ProjectionMatrix, p3VS);
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
    
/*    
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
*/
    
    float3 surfaceColor = ambientColor;
    return float4(surfaceColor, 1.0f);
}
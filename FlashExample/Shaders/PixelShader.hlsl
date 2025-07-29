#include "RootSignature.hlsl"
#include "Pipeline.hlsli"

[RootSignature(ROOTSIG)]
float4 main(VSOut inPsInput) : SV_Target
{
    return inPsInput.color;
}
cbuffer object : register(b0)
{
    float4x4 gWorldViewProj;
}

void VS(in float3 inPosL : POSITION, out float4 outPosH : SV_POSITION)
{
    outPosH = mul(float4(inPosL, 1.0f), gWorldViewProj);
}
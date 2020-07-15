Texture2D smap : register(t3);  // Bind to register slot 3
SamplerState ssam : register(s1);   // Bind to register slot 1

bool ShadowUnoccluded(const in float4 shadowHomoPos)
{
    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    return spos.z > 1.0f ? true : smap.Sample(ssam, spos.xy).r > spos.z - 0.005f;
}
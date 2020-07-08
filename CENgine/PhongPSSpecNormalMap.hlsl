#include "ShaderOperations.hlsl"
#include "PointLight.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;

float4 main(
    float3 viewFragPos : Position,
    float3 viewNormal : Normal,
    float3 viewTan : Tangent,
    float3 viewBitan : Bitangent,
    float2 tc : Texcoord)
        : SV_Target
{
    // Sample diffuse texture
    float4 dtex = tex.Sample(splr, tc);
    
    #ifdef MASKED
    // Bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // Flip normal when backface
    if(dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    #endif
    
    // Normalize the mesh normal
    viewNormal = normalize(viewNormal);
    
    // Replace normal with mapped if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
    
	// Fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	
    // Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if(specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if(hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    
    // Attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	
    // Diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    
    // Specular reflected
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lv.vToL, viewFragPos, att, specularPower);

    // Final color attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}
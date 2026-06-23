#include "Engine_Shader_Defines.hlsli"
float4x4 g_World[800], g_View, g_Projection;
float4 g_Color;
float4 g_Emissive;

texture2D g_DiffuseTexture;
texture2D g_NoiseTexture;
texture2D g_BlurTexture;
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
  
};

struct VS_OUT
{
    float4 vPostion : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct VS_OUT_BLOOM
{
    float4 vPostion : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};
VS_OUT VS_MAIN(VS_IN In ,uint instanceid :SV_InstanceID)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP,matWorld;
    
    matWorld = g_World[instanceid];
    
    matWV = mul(matWorld, g_View);
    matWVP = mul(matWV, g_Projection);
    
    Out.vPostion = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    return Out;
}
VS_OUT_BLOOM VS_MAIN_BLOOM(VS_IN In, uint instanceid : SV_InstanceID)
{
    VS_OUT_BLOOM Out;
    
    float4x4 matWV, matWVP, matWorld;
    
    matWorld = g_World[instanceid];
    
    matWV = mul(matWorld, g_View);
    matWVP = mul(matWV, g_Projection);
    
    Out.vPostion = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), matWorld);
    Out.vProjPos = Out.vPostion;
    Out.vTexcoord = In.vTexcoord;
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_IN_BLOOM
{
    
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};
struct PS_OUT
{
    vector vColor : SV_TARGET0;
};
struct PS_OUT_BLOOM
{
    vector vEmissive : SV_TARGET0;
    
};
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vDiffuse;
    return Out;
}
PS_OUT_BLOOM PS_MAIN_BLOOM(PS_IN_BLOOM In)
{
    PS_OUT_BLOOM Out;
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vNoise = saturate(vNoise + 0.3f);
    vector vColor = lerp(vNoise, g_Emissive, 0.6f);

     Out.vEmissive = vColor;
    return Out;
}
technique11 DefaultTechnique
{
        
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }
    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN_BLOOM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();

    }
}





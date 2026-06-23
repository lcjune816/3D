#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture , g_NoiseTexture;
float g_Time;
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 pos :      POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 texcoord : TEXCOORD0;
    
};

struct VS_OUT {
	float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 texcoord : TEXCOORD0;
}; 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT output ;
	
    float4x4 matWV, matWVP;
	
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	  
    output.pos = mul(float4(In.pos, 1.f), matWVP);
    output.texcoord = In.texcoord;
    output.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
 
  
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vtexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
};


PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out;
    In.vtexcoord.y -= 0.8f * g_Time;
    In.vtexcoord.x += 0.9f * g_Time;
    vector Diffuse = g_DiffuseTexture.Sample(LinearSampler, In.vtexcoord);
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vtexcoord);
       
    float fS = step(0.4f, vNoise) ;
    if (Diffuse.r < 0.1f)
        discard;
    Out.vDiffuse = Diffuse + float4(0.2f, 0.4f, 2.f, 1); // * fS;
    
    return Out;
}
PS_OUT PS_MAIN_HANDSPARK(PS_IN In)
{
    PS_OUT Out;
  
// 2. Y축의 중심(0.5)을 기준으로 확대/축소하고 이.
    In.vtexcoord.y -= 0.8f * g_Time;
    In.vtexcoord.x += 0.9f * g_Time;
    
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vtexcoord);
    
       return Out;
}
technique11 DefaultTechnique
{

    pass Emissive
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //vsMain에있는거를 컴파일 해라
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();

    }
    pass HamdSpark
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //vsMain에있는거를 컴파일 해라
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HANDSPARK();

    }
}

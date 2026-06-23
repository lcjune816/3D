#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

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
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
	float2 texcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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
    output.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    output.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    output.vProjPos = output.pos;
  
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 texcoord : TEXCOORD0;
    
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라  
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.texcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.texcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    Out.vDiffuse = vMtrlDiffuse;
    
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
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
    pass DefaultPassAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

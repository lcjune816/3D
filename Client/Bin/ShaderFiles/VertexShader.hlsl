#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

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
    float2 texcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라  
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.texcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
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
}

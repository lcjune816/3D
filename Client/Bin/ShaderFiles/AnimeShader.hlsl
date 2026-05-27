#include "Engine_Shader_Defines.hlsli"
float4x4 g_World, g_View , g_Projection, g_Bone[512];
Texture2D g_Diffuse;
sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 pos :      POSITION;
    float2 texcoord : TEXCOORD0;
    uint4  vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VOut VS_MAIN(VS_IN In)
{
    VOut output = (VOut)0;
	
    matrix matWV, matWVP;
	
    matWV = mul(g_World ,g_View);
    matWVP = mul(matWV, g_Projection);
	
   float fHeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
   
    matrix BoneMatirx = g_Bone[In.vBlendIndex.x] * In.vBlendWeight.x
   + g_Bone[In.vBlendIndex.y] * In.vBlendWeight.y
   + g_Bone[In.vBlendIndex.z] * In.vBlendWeight.z 
   + g_Bone[In.vBlendIndex.w] * fHeightW;
  
  
   vector vPosition = mul(float4(In.pos, 1.f), BoneMatirx);
   
   vPosition = mul(vPosition, matWVP);
   
   output.pos = vPosition;
   output.texcoord = In.texcoord;
   output.vWorldPos = mul(vector(In.pos, 1.f), g_World);
   output.vProjPos = vPosition;
    
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 textureColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    Out.textureColor = textureColor;
    return Out;
}

technique11 DefaultTechnique
{
        
    pass DefaultPass
    {
        //vsMain에있는거를 컴파일 해라
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }
}

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
    float3 pos : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 texcoord : TEXCOORD0;
    uint4  vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VOut
{
    float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
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
    output.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_World));
   output.vWorldPos = mul(vector(In.pos, 1.f), g_World);
    output.vProjPos = output.vWorldPos;
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
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
    vector vMtrlDiffuse = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    if(vMtrlDiffuse.a <0.3f)
        discard;
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5, 0.f);
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    return Out;
}

technique11 DefaultTechnique
{
        
    pass DefaultPass
    {
        //vsMain에있는거를 컴파일 해라
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }
}

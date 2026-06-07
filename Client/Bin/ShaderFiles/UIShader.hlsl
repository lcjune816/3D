#include "Engine_Shader_Defines.hlsli"
float4x4 g_World, g_View , g_Projection;
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
    float3 vNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
    
};

struct VS_OUT {
	float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 texcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
}; 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT output = (VS_OUT) 0;
	
    float4x4 matWV, matWVP;
	
    matWV = mul(g_World, g_View);
    matWVP = mul(matWV, g_Projection);
	  
   
   output.pos = mul(float4(In.pos, 1.f), matWVP);
   output.texcoord = In.texcoord;
   output.vWorldPos = mul(vector(In.pos, 1.f), g_World);
   
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 textureColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    textureColor *= float4(1.f, 1.f, 1.f, 0.5f);
    if (textureColor.a < 0.1f)
        discard;
    Out.textureColor = textureColor;
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

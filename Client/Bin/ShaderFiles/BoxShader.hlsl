#include "Engine_Shader_Defines.hlsli"
float4x4 g_World, g_View, g_Projection, g_ArrayWorld[512];
float4 g_ArrayColor[512];
vector g_Color;
bool   g_bChoice = true;

struct VS_IN
{
    float3 pos : POSITION;
    float4 Color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 Color : COLOR;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In,uint iInstanceId : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
	
    float4x4 matWV, matWVP;
    if (g_bChoice)
    {
        matWV = mul(g_World, g_View);
        matWVP = mul(matWV, g_Projection);   
    }
    else
    {
        In.Color = g_ArrayColor[iInstanceId];
        matWV = mul(g_ArrayWorld[iInstanceId], g_View);
        matWVP = mul(matWV, g_Projection);
    }
   
    output.pos = mul(float4(In.pos, 1.f), matWVP);
    output.Color = In.Color;
    output.vWorldPos = mul(vector(In.pos, 1.f), g_World);
    output.vProjPos = output.pos;
    
    return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 Color : COLOR;
};

struct PS_OUT
{
    vector Color : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    Out.Color =  g_Color;
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

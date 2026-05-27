#include "Engine_Shader_Defines.hlsli"
float4x4 g_World[800], g_View, g_Projection;
float4 g_Color;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
  
};

struct VS_OUT
{
    float4 vPostion : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor =g_Color;
    
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
        PixelShader = compile ps_5_0 PS_MAIN();

    }
}





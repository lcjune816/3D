#include "Engine_Shader_Defines.hlsli"
float4x4  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
float4 g_Color;
vector g_vCamePosition;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VS_IN
{
    float3 vPosition : POSITION;
 
    row_major float4x4 TransformMatrix : WORLD;
    
    float4 vTexcoord : TEXCOORD0;
    
    float2 vLifeTime : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize    : PSIZE;
    float4 vTexcoord : TEXCOORD0;
    
    float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(length(In.TransformMatrix._11_12_13), length(In.TransformMatrix._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    Out.vTexcoord = In.vTexcoord;
     
    return Out;
}


struct GS_IN
{
    
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};


struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};


//Gs가 최대 6개의 버텍스를 출력하수있음
//primitive = point      삼각형으로 내보내라
[maxvertexcount(6)]             
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = g_vCamePosition.xyz - In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
		//x y          z          w 
		// 최소      x최대      y최대   아 햇 갈 려어ㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓ
    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP); // 00
    Out[0].vTexcoord = float2(In[0].vTexcoord.x, In[0].vTexcoord.y);
    Out[0].vLifeTime = In[0].vLifeTime;

    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP); //10
    Out[1].vTexcoord = float2(In[0].vTexcoord.z, In[0].vTexcoord.y);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP); //11
    Out[2].vTexcoord = float2(In[0].vTexcoord.z, In[0].vTexcoord.w);
    Out[2].vLifeTime = In[0].vLifeTime;

    
    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);//01
    Out[3].vTexcoord = float2(In[0].vTexcoord.x, In[0].vTexcoord.w);
    Out[3].vLifeTime = In[0].vLifeTime;

    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord)* g_Color;
   
    if (Out.vColor.a < 0.1f)
        discard;
    return Out;
}

technique11 DefaultTechnique
{
        
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //vsMain에있는거를 컴파일 해라
       
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }
}

#include "Engine_Shader_Defines.hlsli"
float4x4 g_World, g_View , g_Projection;
Texture2D g_Diffuse;
Texture2D g_NoiseTexture;
float g_Time;
float4 g_vColor;
float4 g_TexCoord;
sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 pos :      POSITION;
    float2 texcoord : TEXCOORD0;
    
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
}; 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT output = (VS_OUT) 0;
	
    float4x4 matWV, matWVP;
	
    matWV = mul(g_World, g_View);
    matWVP = mul(matWV, g_Projection);
	  
   
   output.pos = mul(float4(In.pos, 1.f), matWVP);
   output.texcoord = In.texcoord;
   
	return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector textureColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    if (textureColor.a < 0.1f)
        discard;
       Out.textureColor = textureColor;
    return Out;
}

PS_OUT PS_MAIN_FADEIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
   
    vector vColor = vector(lerp(textureColor.rgb, float3(0, 0, 0), g_Time), 1.f) ;
    
    Out.textureColor = vColor;
    if (Out.textureColor.r > 0.1f)
        discard;
    return Out;
}

PS_OUT PS_MAIN_FADEOUT(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
   
    vector vColor = vector(lerp(float3(0, 0, 0), textureColor.rgb, g_Time), 1.f);
    float fDiscard = 0.1f - (0 + 0.1f) * g_Time;
    if (vColor.r > fDiscard)
        discard;
    Out.textureColor = vColor;
    return Out;
}
PS_OUT PS_MAIN_NOTICE_FADEIN(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.texcoord);
    
    float Time = saturate(g_Time / 3.f);
    
  
    if (vNoise.r > Time)
        discard;
    
    Out.textureColor = textureColor;
    
    return Out;
}
PS_OUT PS_MAIN_NOTICE_FADEOUT(PS_IN In)
{
    PS_OUT Out;
    //xy 좌표에 있는 색상 rgb 값을 가지고와라
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    vector vNoise = g_NoiseTexture.Sample(DefaultSampler, In.texcoord);
    
    float Time = saturate(g_Time / 3.f);
    
    if (vNoise.r > 1.f - Time)
        discard;
    
    Out.textureColor = textureColor;
    
    return Out;
}
PS_OUT PS_MAIN_LOADING(PS_IN In)
{
    PS_OUT Out;
    
    
    In.texcoord.x = In.texcoord.x *(1 / 8.f) + g_TexCoord.z; //0.125
    In.texcoord.y = In.texcoord.y * (1 / 4.f) + g_TexCoord.w; //0.25
   
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord);
    // 0 0 10
    // 0 1 11
    

    textureColor.a = g_TexCoord.x;
    Out.textureColor = textureColor;
    return Out;
}
PS_OUT PS_MAIN_BLACK(PS_IN In)
{
    PS_OUT Out;
    Out.textureColor = float4(0, 0, 0, 0);
    return Out;
}
PS_OUT PS_MAIN_DEADSCREEN(PS_IN In)
{   
    PS_OUT Out;
    float vDiffuse = g_Diffuse.Sample(DefaultSampler, In.texcoord).r;
    vDiffuse = 1.f - vDiffuse;
    vector Color = float4(0.61, 0.013, 0.042, 1.f);
   vector vLast = Color * vDiffuse * 0.7f;
    if (vLast.r < 0.1)
        discard;
    Out.textureColor = vLast;
    
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
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }
    pass FadeIN
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FADEIN();

    }
    pass FadeOUT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FADEOUT();

    }
    pass Loading
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LOADING();

    }
    pass Black
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLACK();

    }

    pass DeadScreen
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEADSCREEN();
    }

    pass NoticeFadeIn
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOTICE_FADEIN();

    }
    pass NoticeFadeOut
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOTICE_FADEOUT();

    }
}

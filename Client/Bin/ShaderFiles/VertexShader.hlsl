#include "Engine_Shader_Defines.hlsli"
float4x4 g_World, g_View , g_Projection;
Texture2D g_Diffuse;
float4 g_Color = { 1.f, 1.f, 1.f, 1.f };
uint g_iCheck;
struct tagLight
{
//빛의 방향?
//오른쪽 + 위쪽에서아 애라로 + 앞쪽 방향으로
    float4 vLightDIr;
    
//빛의 기본 색상 지금은 흰색임
    float4 vLightDiffuse;
    
//환경광 색 방향없이 전체적으로 깔리는 밝기
//전등을 꺼도 살짝은 보이는데 그 최소한의 밝기
    float4 vLightAmbient;
  
//빛의 하이라이트 색 반짝이는 부분
    float4 vLightSpecular;
//오브젝트 머테리얼 뭐드라 난반사랑 정반사
//빛을 얼마나 흡수/ 반사할것인가 보통 lightAmbient(환경광)과 곱해서 계산함
    float4 vMtrlAmbient;
//물체의 매끈함 거친 나무토막 같은거는 반짝임이 거의 없을거임
    float4 vMtrlSpecular; //재질의 반짝임 세기 크면 반짝임이 강하게 보임
};

tagLight g_tagLight;

vector g_vCamPosition;

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
   output.vNormal = mul(float4(In.vNormal, 0.f), g_World);
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
    float4 textureColor = g_Diffuse.Sample(DefaultSampler, In.texcoord) * g_Color;
    //ambient는 환경광 빛이 닿지 않는 곳도 아주 캄캄하지 않게
    //기본적으로 깔아주는 빛
    
    vector vShade = max(dot(normalize(g_tagLight.vLightDIr) * -1.f, normalize(In.vNormal)), 0.f) +
            (g_tagLight.vLightAmbient * g_tagLight.vMtrlAmbient);
    
   
    vector vReflect = reflect(normalize(g_tagLight.vLightDIr), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
   
     //반사된 빛의 방향 reflect 내눈의 방향 Look이 정확히 일치할떄
    //가장 밝게 빛남 매끈한 표면에 맺히는 하이라이트 같은 반짝임을 만듬
    //pow의 값이 커질수록 범위가 좁고 날카로워진다
    float fSpecular = pow(
    max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
   if (textureColor.a < 0.1f)
        discard;
    
    //sature 이거는 0보다 작으면 0 1보다 크면 1로 만들어줌
    //어떤건 더하고 어떤건 곱하고 기준을 잘 모르겠네
        Out.textureColor = g_tagLight.vLightDiffuse * textureColor * saturate(vShade) +
                    (g_tagLight.vLightSpecular * g_tagLight.vMtrlSpecular) * fSpecular;
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

        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


    }
}

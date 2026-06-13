#include "Engine_Shader_Defines.hlsli"


float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInverse, g_ProjMatrixInverse;
texture2D g_Texture;

texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;

vector g_vLightPos;
float2 g_fLightRange;
float g_fLightAngleRange;
vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
vector g_vLightAngle;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


vector g_vCamPosition;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/* 투영변환 -> W나누기 */ 
/* 뷰포트로 변환해준다 */ 
/* 래스터라이즈 : 픽셀의 정보가 생성된다. */ 
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    vector vBackBuffer : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) +
    (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    //2차원 투영 공간상의 위치
    // 0 -> -1 
    // 1 -> 1 NDC 좌표계로 변환 하는거
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    // 0 -> 1
    // 1 -> -1
    // 다렉 함수는 w 나누기까지 진행을 해줘서 NDC가 되어있음
    //그래서 프로젝션 뷰스페이스로 돌리려면 저거 해줘야댐 z나누기전으로 해줘야ㅕ댐
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ; // 뷰로 돌리기
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) *
    pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))),30.f);
    return Out;
}
PS_OUT_LIGHT    PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fLightRange.x - fDistance) / g_fLightRange.x);
    
    Out.vShade = (g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) +
    (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;
    
    Out.vSpecular = (g_vLightSpecular* g_vMtrlSpecular) 
    * pow(saturate(dot(normalize(vReflect) * -1.f,
    normalize(vLook))), 30.f) * fAtt;

    return Out;
}
PS_OUT_LIGHT PS_MAIN_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    vector vLook = vWorldPos - g_vCamPosition;
    
    
    float fDistance = length(vLightDir);
    
    
    float fAttLen = saturate((g_fLightRange.x - fDistance) / g_fLightRange.x);
    
    
    float fAtt = saturate((dot(normalize(vLightDir), normalize(g_vLightDir)) - g_fLightAngleRange) / g_fLightAngleRange);
    
    if (fDistance < g_fLightRange.y)
    {
        Out.vShade = 0.001f;

    }else
    {
        Out.vShade = (g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) +
        (g_vLightAmbient * g_vMtrlAmbient))) * fAtt * fAttLen;
    }
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
  
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular)
    * pow(saturate(dot(normalize(vReflect) * -1.f,
    normalize(vLook))), 30.f) * fAtt * fAttLen;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LINE(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewSpaceZ = vDepthDesc.y * 1000.f;
    vWorldPos = vWorldPos * fViewSpaceZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    
    //A    -     B 거리
    //B    -     A 거리 끼리 내적해서 닿은거로 하면?
    vector vLightDir = vWorldPos - g_vLightPos;
    vector vLook = vWorldPos - g_vCamPosition;
    
    vector vLightEndPos = g_vLightPos + g_vLightDir * g_fLightRange.x;
    
    vector vLightEndDir = vLightEndPos - g_vLightPos;
    
    float fDot = saturate(dot(vLightDir, vLightEndDir) / dot(vLightEndDir, vLightEndDir));
    
    vector vLastPos = vWorldPos - (g_vLightPos + vLightEndPos * fDot);
    float fLentgh = length(vLastPos);
    float fAtt = 1.0f / (1.f + fLentgh * fLentgh);
    
    vector vLastDir = normalize(vLastPos - vWorldPos);
    
    Out.vShade = ((g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) +
    (g_vLightAmbient * g_vMtrlAmbient))) * fAtt) + 0.01f;
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
  
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f,
    normalize(vLook))), 256.f) * fAtt;

    return Out;
}
PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if(0.f == vDiffuse.a)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vSPecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vBackBuffer = vDiffuse * vShade + vSPecular;
    
    return Out;

}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass SpotLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOTLIGHT();
    }

    pass Line
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ZDisable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LINE();
    }
}


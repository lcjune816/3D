	
float4x4 g_World[500], g_View , g_Projection;
Texture2D diffTexture;
vector g_vCamPosition;

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};


BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = add;
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

VS_OUT VS_MAIN(VS_IN In, uint InstanceID : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
	
    float4x4 matWV, matWVP;
    float4x4 matWorld = g_World[InstanceID];
    
    matWV = mul(matWorld, g_View);
    matWVP = mul(matWV, g_Projection);
	  
   
   output.pos = mul(float4(In.pos, 1.f), matWVP);
   output.texcoord = In.texcoord;
    output.vNormal = mul(float4(In.vNormal, 0.f), matWorld);
    output.vWorldPos = mul(vector(In.pos, 1.f), matWorld);
   
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
    float4 textureColor = diffTexture.Sample(DefaultSampler, In.texcoord);
    
   //vector vShade = max(dot(normalize(g_tagLight.vLightDIr) * -1.f, normalize(In.vNormal)), 0.f) +
   //        (g_tagLight.vLightAmbient * g_tagLight.vMtrlAmbient);
   //
   //vector vReflect = reflect(normalize(g_tagLight.vLightDIr), normalize(In.vNormal));
   //vector vLook = In.vWorldPos - g_vCamPosition;
   //
   //float fSpecular = pow(
   //max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
   //
   //f (textureColor.a < 0.1f)
   //    discard;
   //
   //
   ////sature 이거는 0보다 작으면 0 1보다 크면 1로 만들어줌
   ////어떤건 더하고 어떤건 곱하고 기준을 잘 모르겠네
   //Out.textureColor = g_tagLight.vLightDiffuse * textureColor * saturate(vShade) +
   //                (g_tagLight.vLightSpecular * g_tagLight.vMtrlSpecular) * fSpecular;
    
    Out.textureColor = textureColor;
    return Out;
}

technique11 DefaultTechnique
{
        
    pass DefaultPass
    {
        //vsMain에있는거를 컴파일 해라
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


    }
}

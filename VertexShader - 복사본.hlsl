	
matrix World, View , Projection, BoneMatrices[512];


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

VOut main(VS_IN In)
{
    VOut output = (VOut)0;
	
    matrix matWV, matWVP;
	
    matWV = mul(World ,View);
    matWVP = mul(matWV, Projection);
	
   float fHeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
   
   matrix BoneMatirx = BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
   + BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
   + BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z 
   + BoneMatrices[In.vBlendIndex.w] * fHeightW;
  
  
   vector vPosition = mul(float4(In.pos, 1.f), BoneMatirx);
   
   vPosition = mul(vPosition, matWVP);
   
   output.pos = vPosition;
   output.texcoord = In.texcoord;
   output.vWorldPos = mul(vector(In.pos, 1.f), World);
   output.vProjPos = vPosition;
    
	return output;
}

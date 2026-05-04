
struct VS_IN
{
    float4 pos : SV_POSITION;
    float4 vWorldPos : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 color : COLOR;
};
float4 main(VS_IN In) : SV_TARGET
{
	return In.color;
}
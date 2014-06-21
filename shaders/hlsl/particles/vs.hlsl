cbuffer constants
{
	matrix Proj;
	matrix ModelView;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float2 Pos : POSITION;
	float2 UV: TEXCOORD0;
	float3 ParticlePos : TEXCOORD1;
	float3 Speed: TEXCOORD2;
	float4 Color: TEXCOORD3;
	float LifeTime : TEXCOORD4;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 particlePos = float4(input.ParticlePos.x, input.ParticlePos.y, input.ParticlePos.z, 1.0f);
	float4 pos = float4(input.Pos.x, input.Pos.y, 0.f, 0.f);

	float4 worldPos = mul(ModelView, particlePos);
	output.Pos = mul(Proj, worldPos + pos);

	output.UV = input.UV;
	output.Color = input.Color;
	return output;
}
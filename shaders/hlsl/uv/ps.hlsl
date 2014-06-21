struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv: TEXCOORD0;
};


Texture2D tex : register(t0);

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

float4 main(VS_OUTPUT output) : SV_TARGET
{
	return tex.Sample(Sampler, output.uv);
}
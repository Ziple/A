struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 uv: TEXCOORD0;
	float4 lightDir: TEXCOORD1;
};


Texture2D normalMap : register(t0);

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

float4 main(VS_OUTPUT output) : SV_TARGET
{
	float4 lightCol = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 out_Color;

	float3 norm = 2.0f * normalMap.Sample(Sampler, output.uv).rgb - 1.0f;

	float sqrLightDist = dot(output.lightDir, output.lightDir);
	float trLightDist = pow(sqrLightDist, -1.5f);
	float ext = dot(trLightDist*output.lightDir.xyz, normalize(norm));
	float lightPow = 100.f;

	out_Color.rgb = saturate(lightPow*ext) * lightCol.rgb * output.color.rgb;
	out_Color.a = 1.0f;

	return out_Color;
}
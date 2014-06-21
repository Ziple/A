cbuffer Matrices
{
	matrix projection;
	matrix modelview;
};

struct VS_INPUT
{
	float3 in_Vertex : POSITION;
	float2 in_UV : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 uv: TEXCOORD0;
	float4 lightDir: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPos = float4(input.in_Vertex.x, input.in_Vertex.y, input.in_Vertex.z, 1.0f);

	float4 camPos = mul(modelview, worldPos);
	output.pos = mul(projection, camPos);
	output.uv = input.in_UV;

	float4 lightPos = float4(0.f, 10.f, 0.f, 1.f);
	output.lightDir = lightPos - worldPos;

	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return output;
}

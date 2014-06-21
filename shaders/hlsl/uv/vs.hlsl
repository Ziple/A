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
	float2 uv: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPos = float4(input.in_Vertex.x, input.in_Vertex.y, input.in_Vertex.z, 1.0f);
	float4 camPos = mul(modelview, worldPos);
	output.pos = mul(projection, camPos);
	output.uv = input.in_UV;

	return output;
}

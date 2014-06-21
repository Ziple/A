cbuffer Matrices
{
	matrix projection;
	matrix modelview;
};

struct VS_INPUT
{
	float4 in_Vertex : POSITION;
	float4 in_Normal : NORMAL;
	float4 in_Center : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float4 norm: TEXCOORD0;
	float4 lightDir: TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;

	float4 worldPos = mul( modelview, (0.1f * input.in_Vertex + input.in_Center) );
	output.pos = mul( projection, worldPos );

	output.norm = input.in_Normal;

	float4 lightPos = float4(0.f, 0.f, -10.f, 1.f);
	output.lightDir = worldPos - lightPos;

	float zNear = 0.1f;
	float zFar = 100.0f;

	output.color = float4((zFar - worldPos.z) / (zFar - zNear), 1.0f-(worldPos.z - zNear) / (zFar - zNear), 0.0f, 1.0f);

	return output;
}

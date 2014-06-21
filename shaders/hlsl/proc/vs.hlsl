struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_OUTPUT main( float3 pos : POSITION )
{
	VS_OUTPUT output;
	output.pos = float4( pos.x, pos.y, pos.z, 1.0f);

	float2 uv = 0.5f * pos.xy;
	uv.y *= -1.0f;
	uv += 0.5f;

	output.uv = uv;
	return output;
}
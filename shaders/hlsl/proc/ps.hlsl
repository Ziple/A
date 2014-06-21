float4 proc(float2 uv)
{
	return float4(uv.x, uv.y, 0.0f, 1.0f);
}

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main( VS_OUTPUT vsoutput ) : SV_TARGET
{
	return proc(vsoutput.uv);
}
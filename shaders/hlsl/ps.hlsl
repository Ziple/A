struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float4 norm: TEXCOORD0;
	float4 lightDir: TEXCOORD1;
};

float4 main( VS_OUTPUT output ) : SV_TARGET
{
	float4 lightCol = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 out_Color;
	out_Color.rgb = saturate( dot(normalize(output.lightDir.xyz), normalize(output.norm.xyz)) ) * lightCol.rgb * output.color.rgb;
	out_Color.a = 1.0f;

	return out_Color;
}
in vec4 norm;
in vec4 color;
in vec4 lightDir;

out vec4 out_Color;

void main()
{
    vec3 lightCol = vec3( 1.f, 1.f, 1.f );

    out_Color.rgb = clamp( dot( normalize(lightDir), normalize(norm.xyz) ), 0.f, 1.0f ) * lightCol.rgb * color.rgb;
	out_Color.a = 1.0f;
}

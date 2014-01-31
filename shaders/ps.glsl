in vec4 norm;
in vec4 color;
in vec4 lightDir;

out vec4 out_Color;

void main()
{
    vec4 lightCol = vec4( 1.f, 1.f, 1.f, 1.0f );

    out_Color = clamp( dot( normalize(lightDir), normalize(norm) ), 0.f, 1.0f ) * lightCol * color;
}

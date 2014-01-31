uniform mat4 modelview;
uniform mat4 projection;

in vec4 in_Vertex;
in vec4 in_Normal;
in vec4 in_Center;
in vec4 in_Color;

out vec4 norm;
out vec4 color;
out vec4 lightDir;

void main()
{
    vec4 worldPos = modelview * (in_Vertex + in_Center);
    gl_Position = projection * worldPos;
    norm = in_Normal;

    vec4 lightPos = vec4(0.f,0.f, -10.f, 1.f);
    lightDir = worldPos - lightPos;

    color = in_Color;
}

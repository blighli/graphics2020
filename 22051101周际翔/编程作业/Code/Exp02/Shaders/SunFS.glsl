#version 330 core
uniform sampler2D sun_tex;
in vec2 uv;
out vec4 frag_color;
void main()
{
    frag_color = texture(sun_tex, uv);
}
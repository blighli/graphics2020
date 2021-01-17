#version 330 core
in vec2 TexCoords;
in vec3 outColor;
layout (location = 0) out vec4  color;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{  
color =mix(texture(texture1, TexCoords), texture(texture2, TexCoords), 0.4);
}

#version 460 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texTure1;
uniform sampler2D texTure2;

void main()
{
   FragColor = mix(texture(texTure1, TexCoord),texture(texTure2, TexCoord),0);
   //FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
}
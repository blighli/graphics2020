#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
//uniform sampler2D texture2;
//unofprm sampler2D texture3;
void main()
{
	FragColor = texture(texture1, TexCoord);
}
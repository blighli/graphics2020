#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoor;
out vec2 TexCoords;
out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
//gl_Position =transform*vec4(position, 1.0);
gl_Position = projection * view * model * vec4(position, 1.0f);
TexCoords = texCoor;  
outColor = color; 
}

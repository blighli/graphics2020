#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; 
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);

    //ÎÆÀí×ø±ê
    TexCoords = aTexCoords;    

    FragPos = vec3(model * vec4(aPos,1.0));
    Normal = aNormal;
}
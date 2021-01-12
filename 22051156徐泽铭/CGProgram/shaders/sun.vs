#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

    TexCoords = aTexCoord;
    mat4 mvp = projection * view * model;

    gl_Position = mvp * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = vec3(transpose(inverse(model)) * vec4(aNormal, 1.0f));
    Normal = -Normal;


}
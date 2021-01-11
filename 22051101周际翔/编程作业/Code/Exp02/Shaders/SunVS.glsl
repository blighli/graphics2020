#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
layout(location = 0) in vec3 pos_in;
out vec4 pos;
void main()
{
    pos = vec4(pos_in, 1.0f);
    gl_Position = projection * view * model * pos;
}
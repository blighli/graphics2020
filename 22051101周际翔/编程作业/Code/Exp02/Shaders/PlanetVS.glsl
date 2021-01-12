#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
layout(location = 0) in vec3 pos_in;
out vec4 pos;
out vec4 world_pos;
void main()
{
    pos = vec4(pos_in, 1.0f);
    world_pos = model * pos;
    gl_Position = projection * view * world_pos;
}
#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv_in;
out vec2 uv;
void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    uv = uv_in;
}
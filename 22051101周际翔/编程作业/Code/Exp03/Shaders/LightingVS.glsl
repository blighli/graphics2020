#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 world_to_model;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;

out vec2 fs_texcoord;
out vec3 fs_normal_world;
out vec3 fs_position_world;
out vec3 fs_tangent_world;
out vec3 fs_binormal_world;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fs_texcoord = texcoord;
    fs_position_world = (model * vec4(position, 1.0f)).xyz;
    fs_normal_world = normalize(normal * mat3(world_to_model));
    fs_tangent_world = normalize(mat3(model) * tangent);
    fs_binormal_world = normalize(cross(fs_normal_world, fs_tangent_world));
}
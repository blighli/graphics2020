#version 330 core
in vec2 fs_texcoord;
in vec3 fs_normal_world;
in vec3 fs_position_world;
in vec3 fs_tangent_world;
in vec3 fs_binormal_world;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
uniform sampler2D normal_tex;
uniform vec3 camera_pos;
uniform vec3 light_pos[8];
uniform vec3 light_color[8];
uniform float light_intensity[8];
out vec4 frag_color;

void main()
{
    vec2 texcoord = vec2(fs_texcoord.x, 1.0f - fs_texcoord.y);
    vec3 normal_map = texture(normal_tex, texcoord).xyz * 2.0f - 1.0f;
    mat3 tbn_world = mat3(normalize(fs_tangent_world), normalize(fs_binormal_world), normalize(fs_normal_world));
    vec3 world_normal = normalize(tbn_world * normal_map);
    float specular_factor = texture(specular_tex, texcoord).x * 0.5f;
    vec3 view_dir = normalize(camera_pos - fs_position_world);
    frag_color = vec4(0.0f);
    for(int i = 0; i < 8; ++i)
    {
        vec3 light_dir = normalize(light_pos[i] - fs_position_world);
        float light_distance = distance(light_pos[i], fs_position_world);
        float brightness = light_intensity[i] / (light_distance * light_distance) * max(0.0f, dot(world_normal, light_dir));
        vec3 diffuse_contrib = brightness * (1.0f - specular_factor) * texture(diffuse_tex, texcoord).xyz;
        vec3 halfway = normalize(light_dir + view_dir);
        vec3 specular_contrib = vec3(brightness * specular_factor * pow(max(0.0f,dot(world_normal,halfway)),10.0f));
        frag_color += vec4(light_color[i] * (diffuse_contrib + specular_contrib), 1.0f);
    }
}
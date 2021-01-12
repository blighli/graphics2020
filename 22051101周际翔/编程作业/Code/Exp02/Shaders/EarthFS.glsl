#version 330 core
uniform sampler2D day_tex;
uniform sampler2D night_tex;
uniform sampler2D cloud_tex;
uniform sampler2D specular_tex;
uniform vec4 camera_pos;
uniform vec4 sun_pos;
uniform mat4 model;
in vec4 pos;
in vec4 world_pos;
out vec4 frag_color;



void main()
{
    vec3 world_dir = normalize(pos.xyz);
    // Sample the texture.
	const float PI = 3.1415926f;
	float h = world_dir.y;
	float texcoord_v = 0.5f - asin(h) / PI; //(PI / 2.0 - asin(H)) / PI;
	vec2 ortho = normalize(vec2(world_dir.x, world_dir.z));
	float ang_h = acos(ortho.x);
	float texcoord_h = ang_h / 2.0f / PI;
	texcoord_h = ortho.y < 0.0f ? 1.0f - texcoord_h : texcoord_h;
    vec2 uv = vec2(texcoord_h, texcoord_v);
    vec3 day_color = texture(day_tex, uv).xyz;
    vec3 night_color = texture(night_tex, uv).xyz;
    vec3 cloud_color = texture(cloud_tex, uv).xyz;
    vec3 specular_color = texture(specular_tex, uv).xyz;

    // Calculate world space normal.
    vec3 world_space_normal = normalize((model * vec4(world_dir, 0.0f)).xyz);

    // Light dot normal
    vec3 light_dir = normalize(sun_pos.xyz - world_pos.xyz);
    vec3 camera_dir = normalize(camera_pos.xyz - world_pos.xyz);
    float ldn = dot(world_space_normal, light_dir);
    vec3 halfway = normalize(light_dir + camera_dir);

    float energy_in = max(ldn, 0.0f);

    vec3 specular_factor = mix(specular_color, vec3(0.0f), cloud_color.r) * 0.5f;

    vec3 diffuse_color = mix(day_color, cloud_color, cloud_color.r);

    vec3 diffuse_contrib = (1.0f - specular_factor) * energy_in * diffuse_color;
    vec3 specular_contrib = specular_factor * energy_in * pow(vec3(max(0.0f,dot(world_space_normal, halfway))), vec3(10.0f));// No specular color.

    vec3 base_color = diffuse_contrib + specular_contrib;

    vec3 night_color_mixed = mix(night_color * max((1.0f - ldn), 0.0f), vec3(0.0f), cloud_color.r);

    vec3 final_color = max(base_color, night_color_mixed);

    frag_color = vec4(final_color, 1.0f);
}
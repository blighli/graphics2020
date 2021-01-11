#version 330 core
uniform sampler2D color_tex;
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
    vec3 tex_color = texture(color_tex, uv).xyz;

    // Calculate world space normal.
    vec3 world_space_normal = normalize((model * vec4(world_dir, 0.0f)).xyz);

    // Light dot normal
    float ldn = dot(world_space_normal, normalize(sun_pos.xyz - world_pos.xyz));
    
    vec3 base_color = tex_color * max(ldn, 0.0f);
    frag_color = vec4(base_color, 1.0f);
}
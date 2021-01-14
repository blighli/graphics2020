#version 330 core
uniform sampler2D skybox_tex;
in vec4 pos;
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
    frag_color = texture(skybox_tex, vec2(texcoord_h, texcoord_v));
}
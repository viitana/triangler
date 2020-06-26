#version 330 core

in vec2 texture_coord;
in vec3 position;

in vec3 normal_ws;
in vec3 normal_os;
flat in vec3 normal_ws_f;
flat in vec3 normal_os_f;

out vec4 color;

uniform vec3 camera_pos;
uniform vec3 light_dir;
uniform bool flat_shading;

uniform sampler2D texSampler;

void main()
{
	vec4 basecolor = texture(texSampler, texture_coord);
	vec3 light = normalize(light_dir);

	vec3 normal_lighting = flat_shading ? normal_ws_f : normal_ws;
	vec4 c = basecolor * max(0.0, dot(normal_lighting, -light));

	color = basecolor;
}

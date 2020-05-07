#version 330 core

in vec4 fragmentColor;
in vec3 position;

in vec3 normal_ws;
in vec3 normal_os;
flat in vec3 normal_ws_f;
flat in vec3 normal_os_f;

out vec4 color;

uniform vec3 camera_pos;
uniform vec3 light_dir;
uniform bool flat_shading;

void main()
{
	vec3 basecolor = vec3(0.9, 0.9, 0.9);
	vec3 light = normalize(light_dir);

	vec3 normal_lighting = flat_shading ? normal_ws_f : normal_ws;
	vec3 c = basecolor * max(0.0, dot(normal_lighting, -light));

	color = vec4(c, 1.0);
}

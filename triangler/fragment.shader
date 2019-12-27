#version 330 core

in vec4 fragmentColor;
in vec3 normal_ws;
flat in vec3 normal_os;
in vec3 position;

out vec4 color;

uniform vec3 camera_pos;
uniform vec3 light_dir;

void main()
{
	color = fragmentColor * max(0, dot(normal_ws, normalize(light_dir)));
	//color = fragmentColor;
	//color = normal_os;
	// = vec3(1, 0, 0) * max(0, dot(normal_ws, normalize(light_dir)));
}

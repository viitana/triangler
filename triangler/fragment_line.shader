#version 330 core

flat in vec4 fragmentColor;
in vec3 position;

out vec4 color;

uniform vec3 camera_pos;

void main()
{
	float len = length(camera_pos - position);
	float exp = log(len);

	float alphamul = 2.7 - exp;
	color = vec4(fragmentColor.xyz, fragmentColor.a * clamp(alphamul, 0.0, 1.2));
}

#version 330 core

layout(location = 0) in vec3 vertexPosition_world;
layout(location = 1) in vec4 vertexColor;

flat out vec4 fragmentColor;

uniform mat4 model_to_world;
uniform mat4 VP;

void main() {
	vec4 pos = VP * model_to_world * vec4(vertexPosition_world, 1);
	gl_Position = pos;

	fragmentColor = vertexColor;
}

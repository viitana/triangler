#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 surfaceNormal;

out vec4 fragmentColor;
out vec3 normal_ws;
flat out vec3 normal_os;
out vec3 position;

uniform mat4 MVP;

void main() {
	vec4 pos = MVP * vec4(vertexPosition_modelspace, 1);
	gl_Position = pos;

	fragmentColor = vertexColor;
	
	normal_ws =  normalize((MVP * vec4(surfaceNormal, 0)).xyz);
	normal_os = surfaceNormal;
	
	position = pos.xyz;
}

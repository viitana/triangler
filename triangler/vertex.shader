#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 surfaceNormal;

out vec4 fragmentColor;
out vec3 position;

out vec3 normal_ws;
out vec3 normal_os;
flat out vec3 normal_ws_f;
flat out vec3 normal_os_f;

uniform mat4 model_to_world;
uniform mat4 VP;

void main() {
	vec4 pos = VP * model_to_world * vec4(vertexPosition_modelspace, 1);
	gl_Position = pos;

	fragmentColor = vertexColor;
	
	normal_ws =  normalize((model_to_world * vec4(surfaceNormal, 0)).xyz);
	normal_ws_f = normal_ws;

	normal_os = surfaceNormal;
	normal_os_f = normal_os;

	position = pos.xyz;
}

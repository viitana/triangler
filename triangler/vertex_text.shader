#version 330 core

// pos.x, pos.y, tex.x, tex.y
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection_text;

void main()
{
	gl_Position = projection_text * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
}

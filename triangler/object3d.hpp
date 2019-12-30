#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "mesh.hpp"

struct Object3D
{
	GLuint vertex_buffer_id = -1;
	GLuint index_buffer_id = -1;
	GLuint color_buffer_id = -1;
	GLuint normal_buffer_id = -1;

	Mesh mesh;
	glm::mat4 transform = glm::mat4(1.0f);

	void Transform(const glm::mat4 m)
	{
		transform = m * transform;
	}

	void SetTransform(const glm::mat4 m)
	{
		transform = m;
	}

	void ClearBuffers()
	{
		glDeleteBuffers(1, &vertex_buffer_id);
		glDeleteBuffers(1, &index_buffer_id);
		glDeleteBuffers(1, &color_buffer_id);
		glDeleteBuffers(1, &normal_buffer_id);
	}

};

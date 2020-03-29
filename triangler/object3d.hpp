#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"

struct Object3D
{
	GLuint vertex_array_id_ = -1;
	GLuint vertex_buffer_id = -1;
	GLuint index_buffer_id = -1;
	GLuint color_buffer_id = -1;
	GLuint normal_buffer_id = -1;

	Mesh mesh;
	glm::mat4 transform = glm::mat4(1.0f);

	glm::vec3 GetPos() const
	{
		return transform * glm::vec4(0);
	}

	void Transform(const glm::mat4 m)
	{
		transform = m * transform;
	}

	void SetTransform(const glm::mat4 m)
	{
		transform = m;
	}

	void Translate(const glm::vec3 v)
	{
		transform = glm::translate(transform, v);
	}

	void ClearBuffers()
	{
		glDeleteBuffers(1, &vertex_buffer_id);
		glDeleteBuffers(1, &index_buffer_id);
		glDeleteBuffers(1, &color_buffer_id);
		glDeleteBuffers(1, &normal_buffer_id);
		glDeleteVertexArrays(1, &vertex_array_id_);
	}

};

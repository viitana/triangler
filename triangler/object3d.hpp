#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "mesh.hpp"

struct Object3D
{
	GLuint vertex_array_id_ = -1;
	GLuint vertex_buffer_id = -1;
	GLuint index_buffer_id = -1;
	GLuint color_buffer_id = -1;
	GLuint normal_buffer_id = -1;

	Mesh mesh;
	glm::mat4 transform_scale = glm::mat4(1.0f);
	glm::mat4 transform_rotation = glm::mat4(1.0f);
	glm::mat4 transform_translation = glm::mat4(1.0f);
	glm::vec4 line_color = glm::vec4(1.f);

	// Get origin in world-space
	glm::vec3 GetOrigin() const
	{
		return GetTransform() * glm::vec4(0);
	}

	const glm::vec3 GetMid() const
	{
		return GetTransform() * glm::vec4(mesh.mid, 1);
	}

	const glm::mat4 GetTransform() const
	{
		return transform_translation * transform_rotation * transform_scale;
	}

	void Translate(const glm::vec3 v)
	{
		transform_translation = glm::translate(transform_translation, v);
	}

	void Translate(const glm::mat4 m)
	{
		transform_translation *= m;
	}

	void Scale(const float factor)
	{
		transform_scale *= glm::scale(glm::vec3(factor));
		mesh.radius *= factor;
	}

	void Rotate(const glm::mat4 m)
	{
		transform_rotation *= m;
	}

	// Assumes global space axis
	void Rotate(const float deg, const glm::vec3 axis)
	{
		transform_rotation = glm::rotate(deg, axis) * transform_rotation;
	}


	void ResetRotation()
	{
		transform_rotation = glm::mat4(1.f);
	}

	void ResetScale()
	{
		transform_scale = glm::mat4(1.f);
	}

	void ResetTranslation()
	{
		transform_translation = glm::mat4(1.f);
	}

	// Transform a direction vector to global space
	// TODO: Apply scale as well
	const glm::vec3 DirectionToGlobal(const glm::vec3 v) const
	{
		return glm::inverse(transform_rotation) * glm::inverse(transform_scale) * glm::vec4(v, 1.f);
	}

	void ClearBuffers()
	{
		glDeleteBuffers(1, &vertex_buffer_id);
		glDeleteBuffers(1, &index_buffer_id);
		glDeleteBuffers(1, &color_buffer_id);
		glDeleteBuffers(1, &normal_buffer_id);
		glDeleteVertexArrays(1, &vertex_array_id_);
	}

	Object3D() {}
	Object3D(Mesh m) : mesh(m) {}
};

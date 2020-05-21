#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderUniform.h"

const GLuint ShaderUniformInterface::GetLocation(const GLuint shader_id) const
{
	return glGetUniformLocation(shader_id, name_.c_str());
}

void ShaderUniformInterface::AttachNoReciprocation(Shader* shader)
{
	shaders_.insert(shader);
}

void ShaderUniformInterface::Attach(Shader* shader)
{
	AttachNoReciprocation(shader);
	shader->AttachNoReciprocation(this);
}

void ShaderUniformInterface::Clean(Shader* shader)
{
	ApplyTo(shader->id_);
}

template <>
void ShaderUniform<int>::ApplyTo(const GLuint shader_id) const
{
	glUniform1i(GetLocation(shader_id), value_);
}

template <>
void ShaderUniform<glm::mat4>::ApplyTo(const GLuint shader_id) const
{
	glUniformMatrix4fv(GetLocation(shader_id), 1, GL_FALSE, glm::value_ptr(value_));
}


template <>
void ShaderUniform<glm::vec3>::ApplyTo(const GLuint shader_id) const
{
	glUniform3fv(GetLocation(shader_id), 1, glm::value_ptr(value_));
}

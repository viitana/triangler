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

template <>
void ShaderUniform<int>::Clean(Shader* shader)
{
	glUniform1i(GetLocation(shader->id_), value_);
}

template <>
void ShaderUniform<glm::mat4>::Clean(Shader* shader)
{
	glUniformMatrix4fv(GetLocation(shader->id_), 1, GL_FALSE, glm::value_ptr(value_));
}

template <>
void ShaderUniform<glm::vec3>::Clean(Shader* shader)
{
	glUniform3fv(GetLocation(shader->id_), 1, glm::value_ptr(value_));
}

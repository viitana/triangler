#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderUniform.h"

const GLint ShaderUniformInterface::GetLocation() const
{
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	return glGetUniformLocation(program, name_.c_str());
}

template <>
void ShaderUniform<int>::Clean()
{
	glUniform1i(GetLocation(), value_);
}

template <>
void ShaderUniform<glm::mat4>::Clean()
{
	glUniformMatrix4fv(GetLocation(), 1, GL_FALSE, glm::value_ptr(value_));
}

template <>
void ShaderUniform<glm::vec3>::Clean()
{
	glUniform3fv(GetLocation(), 1, glm::value_ptr(value_));
}

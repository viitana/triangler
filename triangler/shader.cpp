#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
{
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertex_shader_code, fragment_shader_code;
	if(
		!LoadShaderFile(vertex_file_path, vertex_shader_code) ||
		!LoadShaderFile(fragment_file_path, fragment_shader_code)
	) return;

	const char* vertex_shader_source_ptr = vertex_shader_code.c_str();
	const char* fragment_shader_source_ptr = fragment_shader_code.c_str();

	if (
		CompileShader(vertex_file_path, vertex_shader_id, vertex_shader_source_ptr) == -1 ||
		CompileShader(fragment_file_path, fragment_shader_id, fragment_shader_source_ptr) == -1
		) return;

	GLuint program_id = LinkShaders(vertex_shader_id, fragment_shader_id);

	if (program_id != -1);
	{
		glDetachShader(program_id, vertex_shader_id);
		glDetachShader(program_id, fragment_shader_id);
	}

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	id_ = program_id;
}

const bool Shader::LoadShaderFile(const char* shader_path, std::string& shader_code) const
{
	std::stringstream shader_sstream;
	std::ifstream shader_fstream(shader_path, std::ios::in);

	if (shader_fstream.is_open())
	{
		shader_sstream << shader_fstream.rdbuf();
		shader_code = shader_sstream.str();
		shader_fstream.close();
		return true;
	}
	else
	{
		printf("Unable to open shader file '%s'\n", shader_path);
		getchar();
		return false;
	}
}

const GLuint Shader::CompileShader(const char* shader_path, const GLuint shader_id, const char* source_ptr)
{
	GLint result = GL_FALSE;
	int log_length;

	// Compile
	printf("Compiling shader: '%s'\n", shader_path);
	glShaderSource(shader_id, 1, &source_ptr, NULL);
	glCompileShader(shader_id);

	// Check for errors
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0) {
		std::vector<char> error_msg(log_length + 1);
		glGetShaderInfoLog(shader_id, log_length, NULL, &error_msg[0]);
		printf("%s\n", &error_msg[0]);
		return -1;
	}
	return shader_id;
}

const GLuint Shader::LinkShaders(const GLuint vertex_shader_id, GLuint fragment_shader_id)
{
	printf("Linking program\n");

	GLint result = GL_FALSE;
	int log_length;

	// Link
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	// Check for errors
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0) {
		std::vector<char> ProgramErrorMessage(log_length + 1);
		glGetProgramInfoLog(program_id, log_length, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		return -1;
	}
	return program_id;
}

void Shader::NotifyDirty(ShaderUniformInterface* cleanable)
{
	uniforms_dirty_.insert(cleanable);
}


void Shader::CleanObservees()
{
	glUseProgram(id_);
	for (CleanableBy<Shader>* uniform : uniforms_dirty_)
	{
		uniform->Clean(this);
	}
	uniforms_dirty_.clear();
}

void Shader::AttachNoReciprocation(ShaderUniformInterface* uniform)
{
	uniforms_.insert(uniform);
	NotifyDirty(uniform);
}

void Shader::Attach(ShaderUniformInterface* uniform)
{
	AttachNoReciprocation(uniform);
	uniform->AttachNoReciprocation(this);
}

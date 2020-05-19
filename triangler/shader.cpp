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

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Unable to open %s.\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

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
	uniforms_dirty_.insert(uniform);
}

void Shader::Attach(ShaderUniformInterface* uniform)
{
	AttachNoReciprocation(uniform);
	uniform->AttachNoReciprocation(this);
}

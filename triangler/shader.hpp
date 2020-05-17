#pragma once

#include <string.h>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

class Shader
{
public:
	GLuint id_;

	Shader(const char* vertex_file_path, const char* fragment_file_path);

private:
	const bool LoadShaderFile(const char* shader_path, std::string& shader_cod) const;
	const GLuint CompileShader(const char* shader_path, const GLuint shader_id, const char* source_ptr);
	const GLuint LinkShaders(const GLuint vertex_shader_id, GLuint fragment_shader_id);
};

#pragma once

#include <string.h>
#include <any>
#include <typeindex>
#include <set>

#include "cleanable.hpp"
#include "mutuallyattachable.hpp"
#include "ShaderUniform.h"

class ShaderUniformInterface;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

class Shader : public CleanableObserverOf<ShaderUniformInterface>, public MutuallyAttachableTo<ShaderUniformInterface>
{
public:
	GLuint id_;

	Shader(const char* vertex_file_path, const char* fragment_file_path);

	// Inherited: CleanableObserver
	void NotifyDirty(ShaderUniformInterface* cleanable);
	void CleanObservees();

	// Inherited: MutuallyAttachable
	virtual void AttachNoReciprocation(ShaderUniformInterface* uniform);
	virtual void Attach(ShaderUniformInterface* uniform);

private:
	std::set<ShaderUniformInterface*> uniforms_;
	std::set<ShaderUniformInterface*> uniforms_dirty_;

	// Read shader source from file
	const bool LoadShaderFile(const char* shader_path, std::string& shader_cod) const;
	// Compile shader source
	const GLuint CompileShader(const char* shader_path, const GLuint shader_id, const char* source_ptr);
	// Link shader programs
	const GLuint LinkShaders(const GLuint vertex_shader_id, GLuint fragment_shader_id);
};

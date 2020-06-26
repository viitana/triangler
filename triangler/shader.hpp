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

class Shader :
	public CleanableObserverOf<ShaderUniformInterface>,
	public MutuallyAttachableTo<ShaderUniformInterface>
{
public:
	GLuint id_ = -1;

	Shader(const char* vertex_file_path, const char* fragment_file_path);

	// Load, compile and link the shader program from source file paths
	void Compile();

	// Inherited: CleanableObserver
	void NotifyDirty(ShaderUniformInterface* cleanable) override;
	void CleanObservees() override;

	// Inherited: MutuallyAttachable
	virtual void AttachNoReciprocation(ShaderUniformInterface* uniform) override;
	virtual void Attach(ShaderUniformInterface* uniform) override;

private:
	const char* vertex_file_path_;
	const char* fragment_file_path_;

	std::set<ShaderUniformInterface*> uniforms_;
	std::set<ShaderUniformInterface*> uniforms_dirty_;

	// Read shader source from file
	const bool LoadShaderFile(const char* shader_path, std::string& shader_cod) const;
	// Compile shader source
	const GLuint CompileShaderSource(const char* shader_path, const GLuint shader_id, const char* source_ptr);
	// Link shader programs
	const GLuint LinkShaders(const GLuint vertex_shader_id, GLuint fragment_shader_id);
};

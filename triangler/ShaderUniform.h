#pragma once

#include <string>
#include <set>

#include "shader.hpp"
#include "cleanable.hpp"
#include "mutuallyattachable.hpp"

class Shader;

class ShaderUniformInterface : public MutuallyAttachableTo<Shader>, public CleanableBy<Shader>
{
public:
	ShaderUniformInterface(const std::string name)
		: name_(name) {}

	virtual void ApplyTo(const GLuint shader_id) const = 0;

	// Inherited: MutuallyAttachable
	virtual void AttachNoReciprocation(Shader* shader);
	virtual void Attach(Shader* shader);

	// Inherited: Cleanable
	virtual void Clean(Shader* shader);

	const std::string name_;

protected:
	const GLuint GetLocation(const GLuint shader_id) const;
	std::set<Shader*> shaders_;
private:
	bool changed_ = true;
};

template <typename T>
class ShaderUniform : public ShaderUniformInterface
{
public:
	ShaderUniform(const std::string name, const T value)
		: ShaderUniformInterface(name), value_(value) {}
	void SetValue(const T value)
	{
		value_ = value;
		for (Shader* shader : shaders_)
		{
			shader->NotifyDirty(this);
		}
	}

	// Inherited: ShaderUniformInterface
	// Apply uniform to a given shader id, overridden per-type
	virtual void ApplyTo(const GLuint shader_id) const;

private:
	T value_;
};

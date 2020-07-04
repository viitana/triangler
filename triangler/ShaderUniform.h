#pragma once

#include <string>
#include <vector>

#include "cleanable.hpp"

class ShaderUniformInterface :
	public Cleanable // by Shader
{
public:
	ShaderUniformInterface(const std::string name)
		: name_(name) {}

	// Inherited: Cleanable
	virtual void Clean() override = 0;
	virtual void AddObserver(CleanableObserver* observer) override { observers_.emplace_back(observer); }

	const std::string name_;

protected:
	const GLint GetLocation() const;
	std::vector<CleanableObserver*> observers_;

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
		for (CleanableObserver* shader : observers_)
		{
			shader->NotifyDirty(this);
		}
	}

	// Inherited: ShaderUniformInterface / Cleanable
	virtual void Clean() override;

private:
	T value_;
};

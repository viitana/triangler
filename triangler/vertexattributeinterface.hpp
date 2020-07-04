#pragma once

#include <string>
#include <set>

#include <GL/glew.h>

#include "cleanable.hpp"

class VertexAttributeInterface :
	public Cleanable
{
public:
	VertexAttributeInterface(const std::string name, const GLuint location)
		: name_(name), location_(location) {}

	// Inherited: Cleanable
	virtual void Clean() override = 0;
	virtual void AddObserver(CleanableObserver* observer) override { observers_.emplace_back(observer); }

	const std::string name_;
	GLuint buffer_id_ = -1;

protected:
	const GLuint location_;
	std::vector<CleanableObserver*> observers_;

	virtual const GLint GetAttributeSize() const = 0;
	virtual const GLenum GetType() const = 0;

	virtual const GLboolean GetNormalize() const { return GL_FALSE; }
	virtual const GLsizei GetStride() const { return 0; }
	virtual const void* GetOffset() const { return 0; }

};

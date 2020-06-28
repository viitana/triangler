#pragma once

#include <string>
#include <set>

#include <GL/glew.h>

#include "cleanable.hpp"
#include "mutuallyattachable.hpp"
#include "object3d.hpp"

class Object3D;

class VertexAttributeInterface :
	public MutuallyAttachableTo<Object3D>,
	public CleanableBy<Object3D>
{
public:
	VertexAttributeInterface(const std::string name, const GLuint location)
		: name_(name), location_(location) {}

	// Inherited: MutuallyAttachable
	virtual void AttachNoReciprocation(Object3D* object) override;
	virtual void Attach(Object3D* object) override;

	// Inherited: Cleanable
	virtual void Clean(Object3D* object) = 0;

	const std::string name_;
	GLuint buffer_id_ = -1;

protected:
	std::set<Object3D*> objects_;
	const GLuint location_;

	virtual const GLint GetAttributeSize() const = 0;
	virtual const GLenum GetType() const = 0;

	virtual const GLboolean GetNormalize() const { return GL_FALSE; }
	virtual const GLsizei GetStride() const { return 0; }
	virtual const void* GetOffset() const { return 0; }

};

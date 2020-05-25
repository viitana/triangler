#pragma once

#include <GL/glew.h>

#include "cleanable.hpp"
#include "mutuallyattachable.hpp"

#include "object3d.hpp"

class Object3D;

class VertexAttributeInterface : public MutuallyAttachableTo<Object3D>, public CleanableBy<Object3D>
{
public:
	VertexAttributeInterface(const std::string name, const GLuint location)
		: name_(name), location_(location) {}
	
	// Inherited: MutuallyAttachable
	virtual void AttachNoReciprocation(Object3D* object);
	virtual void Attach(Object3D* object);

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

template <typename T>
class VertexAttribute : public VertexAttributeInterface
{
public:
	VertexAttribute(const std::string name, const GLuint location, const std::vector<T> data)
		: VertexAttributeInterface(name, location), data_(data) {}

	void SetData(const std::vector<T> data)
	{
		data_ = data;
		for (Object3D* object : objects_)
		{
			object->NotifyDirty(this);
		}
	}

	// Inherited: ShaderUniformInterface / Cleanable
	virtual void Clean(Object3D* object)
	{
		if (data_.empty()) return;

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
		glBufferData(
			GL_ARRAY_BUFFER,
			data_.size() * sizeof(T),
			data_.data(),
			GL_STATIC_DRAW
		);

		glEnableVertexAttribArray(location_);
		glVertexAttribPointer(
			location_,
			GetAttributeSize(),
			GetType(),
			GetNormalize(),
			GetStride(),
			GetOffset()
		);
	}

	// Inherited: ShaderUniformInterface
	virtual const GLint GetAttributeSize() const;
	virtual const GLenum GetType() const;

private:
	std::vector<T> data_;
};

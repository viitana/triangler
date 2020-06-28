#pragma once

#include "vertexattributeinterface.hpp"
#include "object3d.hpp"

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

	// Inherited: VertexAttributeInterface / Cleanable
	virtual void Clean(Object3D* object) override;

	// Inherited: VertexAttributeInterface
	virtual const GLint GetAttributeSize() const override;
	virtual const GLenum GetType() const override;

private:
	std::vector<T> data_;
};

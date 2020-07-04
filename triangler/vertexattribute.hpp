#pragma once

#include <vector>

#include "vertexattributeinterface.hpp"
#include <glm/glm.hpp>

template <typename T>
class VertexAttribute : public VertexAttributeInterface
{
public:
	VertexAttribute(const std::string name, const GLuint location, const std::vector<T> data)
		: VertexAttributeInterface(name, location), data_(data) {}

	void SetData(const std::vector<T> data)
	{
		data_ = data;
		for (CleanableObserver* object : observers_)
		{
			object->NotifyDirty2(this);
		}
	}

	// Inherited: VertexAttributeInterface / Cleanable
	virtual void Clean2(void* info) override
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

	// Inherited: VertexAttributeInterface
	virtual const GLint GetAttributeSize() const override;
	virtual const GLenum GetType() const override;

private:
	std::vector<T> data_;
};

// vec2
template <>
const GLint VertexAttribute<glm::vec2>::GetAttributeSize() const;

template <>
const GLenum VertexAttribute<glm::vec2>::GetType() const;

// vec3
template <>
const GLint VertexAttribute<glm::vec3>::GetAttributeSize() const;

template <>
const GLenum VertexAttribute<glm::vec3>::GetType() const;

// vec4
template <>
const GLint VertexAttribute<glm::vec4>::GetAttributeSize() const;

template <>
const GLenum VertexAttribute<glm::vec4>::GetType() const;

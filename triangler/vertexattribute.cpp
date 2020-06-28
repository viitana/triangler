#include <glm/glm.hpp>

#include "vertexattribute.hpp"

// All possible VertexAttribute types https://stackoverflow.com/q/495021
template class VertexAttribute<glm::vec2>;
template class VertexAttribute<glm::vec3>;
template class VertexAttribute<glm::vec4>;

template <typename T>
void VertexAttribute<T>::Clean(Object3D* object)
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

// vec2
template <>
const GLint VertexAttribute<glm::vec2>::GetAttributeSize() const { return 2; }
template <>
const GLenum VertexAttribute<glm::vec2>::GetType() const { return GL_FLOAT; }


// vec3
template <>
const GLint VertexAttribute<glm::vec3>::GetAttributeSize() const { return 3; }
template <>
const GLenum VertexAttribute<glm::vec3>::GetType() const { return GL_FLOAT; }

// vec4
template <>
const GLint VertexAttribute<glm::vec4>::GetAttributeSize() const { return 4; }
template <>
const GLenum VertexAttribute<glm::vec4>::GetType() const { return GL_FLOAT; }

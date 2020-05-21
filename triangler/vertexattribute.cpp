#include <glm/glm.hpp>

#include "vertexattribute.hpp"

void VertexAttributeInterface::AttachNoReciprocation(Object3D* object)
{
	objects_.insert(object);
}

void VertexAttributeInterface::Attach(Object3D* object)
{
	AttachNoReciprocation(object);
	object->AttachNoReciprocation(this);
}

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

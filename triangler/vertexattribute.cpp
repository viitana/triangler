#include "vertexattribute.hpp"

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

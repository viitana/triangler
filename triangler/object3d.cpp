#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "object3d.hpp"

void Object3D::Init()
{
	glUseProgram(shader_->id_);

	glGenVertexArrays(1, &vertex_array_id_);
	glBindVertexArray(vertex_array_id_);
}

void Object3D::Draw() const
{
	if (isIndexed()) glDrawArrays(GL_TRIANGLES, 0, mesh.v.size());
	else glDrawElements(GL_TRIANGLES, mesh.t.size(), GL_UNSIGNED_INT, (void*)0);
}

void Object3D::Render()
{
	BindObject();
	CleanObservees(); // Apply dirty attributes
	ApplyUniforms(); // Apply dirty uniforms to shader
	Draw();
}

void Object3D::BindObject() const
{
	glUseProgram(shader_->id_);
	glBindVertexArray(vertex_array_id_);
}

void Object3D::GenBuffer(GLuint* id) const
{
	if (glIsBuffer(*id) == GL_TRUE) glDeleteBuffers(1, id);
	glGenBuffers(1, id);
}

void Object3D::BindData(GLenum target, GLenum buffer, GLsizeiptr size, const void* data)
{
	glBindBuffer(target, buffer);
	glBufferData(target, size, data, draw_mode_);
}

void Object3D::AttachNoReciprocation(VertexAttributeInterface* vertex_attribute)
{
	vertex_attributes_.insert(vertex_attribute);
	NotifyDirty(vertex_attribute);
}

void Object3D::Attach(VertexAttributeInterface* vertex_attribute)
{
	AttachNoReciprocation(vertex_attribute);
	vertex_attribute->AttachNoReciprocation(this);
}

void Object3D::NotifyDirty(VertexAttributeInterface* vertex_attribute)
{
	vertex_attributes_dirty_.insert(vertex_attribute);
}

void Object3D::CleanObservees()
{
	BindObject();
	for (VertexAttributeInterface* vertex_attribute : vertex_attributes_dirty_)
	{
		vertex_attribute->Clean(this);
	}
	vertex_attributes_dirty_.clear();
}

void Object3D::SetMesh(Mesh m)
{
	if (m.v.empty()) return;

	mesh = m;
	BindObject();

	// Vertex pos
	{
		GenBuffer(&vertex_buffer_id);
		BindData(GL_ARRAY_BUFFER, vertex_buffer_id, mesh.v.size() * sizeof(mesh.v[0]), mesh.v.data());
		glEnableVertexAttribArray(VERTEX_POS_ATTRIB_LOCATION);
		glVertexAttribPointer(
			VERTEX_POS_ATTRIB_LOCATION,
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			(void*)0 // offset
		);
	}

	// Vertex colors
	{
		if (mesh.c.empty()) genRandomColors(mesh);
		GenBuffer(&color_buffer_id);
		BindData(GL_ARRAY_BUFFER, color_buffer_id, mesh.c.size() * sizeof(mesh.c[0]), mesh.c.data());
		glEnableVertexAttribArray(VERTEX_COLOR_ATTRIB_LOCATION);
		glVertexAttribPointer(
			VERTEX_COLOR_ATTRIB_LOCATION,
			4, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			(void*)0 // offset
		);
	}

	// Vertex normals
	{
		if (mesh.n.empty()) genNormals(mesh);
		GenBuffer(&normal_buffer_id);
		BindData(GL_ARRAY_BUFFER, normal_buffer_id, mesh.n.size() * sizeof(mesh.n[0]), mesh.n.data());
		glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_LOCATION);
		glVertexAttribPointer(
			VERTEX_NORMAL_ATTRIB_LOCATION,
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			(void*)0 // offset
		);
	}

	if (!mesh.t.empty()) // Vertex indexing
	{
		GenBuffer(&index_buffer_id);
		BindData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id, mesh.t.size() * sizeof(mesh.t[0]), mesh.t.data());
	}
}

const bool Object3D::isIndexed() const
{
	return index_buffer_id == -1;
}

// Get origin in world-space
glm::vec3 Object3D::GetOrigin() const
{
	return GetTransform() * glm::vec4(0);
}

const glm::vec3 Object3D::GetMid() const
{
	return GetTransform() * glm::vec4(mesh.mid, 1);
}

const glm::mat4 Object3D::GetTransform() const
{
	return transform_;
}

void Object3D::UpdateUniforms()
{
	static_cast<ShaderUniform<glm::mat4>*>(uniforms_["model_to_world"])->SetValue(transform_);
}

void Object3D::TransformChanged()
{
	transform_ = transform_translation * transform_rotation * transform_scale;;
	UpdateUniforms();
}

void Object3D::Translate(const glm::vec3 v)
{
	transform_translation = glm::translate(transform_translation, v);
	TransformChanged();
}

void Object3D::Translate(const glm::mat4 m)
{
	transform_translation *= m;
	TransformChanged();
}

void Object3D::Scale(const float factor)
{
	transform_scale *= glm::scale(glm::vec3(factor));
	mesh.radius *= factor;
	TransformChanged();
}

void Object3D::Rotate(const glm::mat4 m)
{
	transform_rotation *= m;
	TransformChanged();
}

// Assumes global space axis
void Object3D::Rotate(const float deg, const glm::vec3 axis)
{
	transform_rotation = glm::rotate(deg, axis) * transform_rotation;
	TransformChanged();
}

void Object3D::ResetRotation()
{
	transform_rotation = glm::mat4(1.f);
	TransformChanged();
}

void Object3D::ResetScale()
{
	transform_scale = glm::mat4(1.f);
	TransformChanged();
}

void Object3D::ResetTranslation()
{
	transform_translation = glm::mat4(1.f);
	TransformChanged();
}

void Object3D::AddUniform(ShaderUniformInterface* uniform)
{
	uniforms_.insert({ uniform->name_, uniform });
	uniform->Attach(shader_);
}

// Clean shader uniforms
void Object3D::ApplyUniforms() const
{
	for (auto const& pair : uniforms_)
	{
		ShaderUniformInterface* uniform = pair.second;
		uniform->Clean(shader_);
	}
}

// Transform a direction vector to global space
const glm::vec3 Object3D::DirectionToGlobal(const glm::vec3 v) const
{
	return glm::inverse(transform_rotation) * glm::inverse(transform_scale) * glm::vec4(v, 1.f);
}

void Object3D::ClearBuffers()
{
	glDeleteBuffers(1, &vertex_buffer_id);
	glDeleteBuffers(1, &index_buffer_id);
	glDeleteBuffers(1, &color_buffer_id);
	glDeleteBuffers(1, &normal_buffer_id);
	glDeleteVertexArrays(1, &vertex_array_id_);
}

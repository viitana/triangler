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
	GLenum primitive_mode = GL_TRIANGLES;
	switch (type_)
	{
		case ObjectType::Mesh: break;
		case ObjectType::Line: primitive_mode = GL_LINES; break;
		case ObjectType::Point: primitive_mode = GL_POINTS; break;
	}

	if (texture_)
	{
		texture_->Bind();
	}

	if (isIndexed())
	{
		glDrawElements(primitive_mode, mesh.t.size(), GL_UNSIGNED_INT, (void*)0);
	}
	else
	{
		glDrawArrays(primitive_mode, 0, mesh.v.size());
	}
}

void Object3D::Render()
{
	BindObject();
	CleanObservees(); // Rebind dirty vertex attributes
	ApplyUniforms(); // Rebind dirty uniforms to shader
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
	// Generate buffers for attributes on attach
	BindObject();
	GenBuffer(&vertex_attribute->buffer_id_);

	vertex_attributes_.insert({ vertex_attribute->name_, vertex_attribute });
	NotifyDirty(vertex_attribute);
}

void Object3D::Attach(VertexAttributeInterface* vertex_attribute)
{
	AttachNoReciprocation(vertex_attribute);
	vertex_attribute->AttachNoReciprocation(this);
}

VertexAttributeInterface* Object3D::GetVertexAttribute(const std::string name)
{
	return vertex_attributes_[name];
}

void Object3D::NotifyDirty(VertexAttributeInterface* vertex_attribute)
{
	vertex_attributes_dirty_.insert(vertex_attribute);
}

void Object3D::CleanObservees()
{
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
		VertexAttribute<glm::vec3>* vertex_positions = new VertexAttribute<glm::vec3>(
			VERTEX_ATTRIB_NAME_POSITION,
			VERTEX_ATTRIB_LOCATION_POSITION,
			mesh.v
		);
		Attach(vertex_positions);
	}

	// Vertex colors
	{
		if (mesh.c.empty()) genRandomColors(mesh);
		VertexAttribute<glm::vec4>* vertex_colors = new VertexAttribute<glm::vec4>(
			VERTEX_ATTRIB_NAME_COLOR,
			VERTEX_ATTRIB_LOCATION_COLOR,
			mesh.c
		);
		Attach(vertex_colors);
	}

	// Vertex normals
	if (type_ == ObjectType::Mesh)
	{
		if (mesh.n.empty()) genNormals(mesh);
		VertexAttribute<glm::vec3>* vertex_normals = new VertexAttribute<glm::vec3>(
			VERTEX_ATTRIB_NAME_NORMAL,
			VERTEX_ATTRIB_LOCATION_NORMAL,
			mesh.n
		);
		Attach(vertex_normals);
	}

	// Vertex indexing
	if (!mesh.t.empty())
	{
		GenBuffer(&index_buffer_id);
		BindData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id, mesh.t.size() * sizeof(mesh.t[0]), mesh.t.data());
	}
}

const bool Object3D::isIndexed() const
{
	return index_buffer_id != -1;
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

void Object3D::AttachNoReciprocation(Texture* tx)
{
	BindObject();

	VertexAttribute<glm::vec2>* tex_coords = new VertexAttribute<glm::vec2>(
		VERTEX_ATTRIB_NAME_TEXTURECOORD,
		VERTEX_ATTRIB_LOCATION_TEXTURECOORD,
		mesh.tx
	);
	Attach(tex_coords);
	texture_ = tx;
}

void Object3D::Attach(Texture* tx)
{
	AttachNoReciprocation(tx);
	tx->AttachNoReciprocation(this);
}

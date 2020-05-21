#pragma once

#include <set>
#include <map>

#include <glm/glm.hpp>

#include "mutuallyattachable.hpp"
#include "cleanable.hpp"

#include "mesh.hpp"
#include "shader.hpp"
#include "vertexattribute.hpp"

#define VERTEX_POS_ATTRIB_LOCATION 0
#define VERTEX_COLOR_ATTRIB_LOCATION 1
#define VERTEX_NORMAL_ATTRIB_LOCATION 2

class VertexAttributeInterface;

class Object3D : public CleanableObserverOf<VertexAttributeInterface>, public MutuallyAttachableTo<VertexAttributeInterface>
{
public:
	//Object3D() {}
	Object3D(Shader* shader) : shader_(shader) {}
	Object3D(Shader* shader, Mesh m) : shader_(shader), mesh(m) {}

	GLuint vertex_array_id_ = -1;

	GLuint vertex_buffer_id = -1;
	GLuint index_buffer_id = -1;
	GLuint color_buffer_id = -1;
	GLuint normal_buffer_id = -1;

	glm::mat4 transform_ = glm::mat4(1.0f);
	glm::mat4 transform_scale = glm::mat4(1.0f);
	glm::mat4 transform_rotation = glm::mat4(1.0f);
	glm::mat4 transform_translation = glm::mat4(1.0f);

	glm::vec4 line_color = glm::vec4(1.f);

	Mesh mesh;

	Shader* shader_ = nullptr;
	std::map<std::string, ShaderUniformInterface*> uniforms_ = {
		{ "model_to_world", new ShaderUniform<glm::mat4>("model_to_world", glm::mat4(1.0f)) },
	};

	// Generate VAO
	void Init();
	// Bind shader & VAO, clean apply uniforms to shader, draw
	void Render();
	// Bind buffer and apply data to the given target location
	void BindData(GLenum target, GLenum buffer, GLsizeiptr size, const void* data);
	// Add a mesh; Binds all vertex data in the mesh and generates colors & normals if missing
	void SetMesh(Mesh m);

	// Get origin in world-space
	glm::vec3 GetOrigin() const;
	const glm::vec3 GetMid() const;
	const glm::mat4 GetTransform() const;

	void Translate(const glm::vec3 v);
	void Translate(const glm::mat4 m);
	void Scale(const float factor);
	void Rotate(const glm::mat4 m);
	void Rotate(const float deg, const glm::vec3 axis); // Assumes global space axis
	void ResetRotation();
	void ResetScale();
	void ResetTranslation();
	// Transform a direction vector to global space
	const glm::vec3 DirectionToGlobal(const glm::vec3 v) const;

	// Set uniform values
	virtual void UpdateUniforms();
	// Add uniform & attach to shader
	void AddUniform(ShaderUniformInterface* uniform);
	// Clean shader uniforms
	void ApplyUniforms() const;

	// Inherited: CleanableObserver
	void NotifyDirty(VertexAttributeInterface* vertex_attribute);
	void CleanObservees();

	// Inherited: MutuallyAttachable
	void AttachNoReciprocation(VertexAttributeInterface* vertex_attribute);
	void Attach(VertexAttributeInterface* vertex_attribute);

private:
	std::set<VertexAttributeInterface*> vertex_attributes_;
	std::set<VertexAttributeInterface*> vertex_attributes_dirty_;

	const GLenum draw_mode_ = GL_STATIC_DRAW;

	// Submit draw call
	void Draw() const;

	// Bind shader & VAO
	void BindObject() const;

	// Generate a buffer if it does not exist
	void GenBuffer(GLuint* id) const;

	const bool isIndexed() const;
	void TransformChanged();
	void ClearBuffers();
};

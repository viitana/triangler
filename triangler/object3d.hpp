#pragma once

#include <set>
#include <unordered_map> 
#include <iostream>

#include <glm/glm.hpp>

#include "attachingto.hpp"
#include "cleanable.hpp"

#include "mesh.hpp"
#include "shader.hpp"
#include "vertexattributeinterface.hpp"
#include "Texture.hpp"

enum class ObjectType {
	Mesh,
	Line,
	Point
};

#define VERTEX_ATTRIB_NAME_POSITION "vertex_position_modelspace"
#define VERTEX_ATTRIB_NAME_COLOR "vertex_color"
#define VERTEX_ATTRIB_NAME_NORMAL "vertex_normal"
#define VERTEX_ATTRIB_NAME_TEXTURECOORD "texture_coordinate"

#define VERTEX_ATTRIB_LOCATION_POSITION 0
#define VERTEX_ATTRIB_LOCATION_COLOR 1
#define VERTEX_ATTRIB_LOCATION_NORMAL 2
#define VERTEX_ATTRIB_LOCATION_TEXTURECOORD 3

class VertexAttributeInterface;
class Texture;

class Object3D :
	public CleanableObserver, // of VertexAttributeInterface
	public AttachingTo<VertexAttributeInterface>,
	public AttachingTo<Texture>
{
public:
	Object3D(ObjectType type, Shader* shader) : type_(type), shader_(shader) {}
	Object3D(ObjectType type, Shader* shader, Mesh m) : type_(type), shader_(shader), mesh(m) {}

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
	std::unordered_map<std::string, ShaderUniformInterface*> uniforms_ = {
		{ "model_to_world", new ShaderUniform<glm::mat4>("model_to_world", glm::mat4(1.0f)) },
	};

	// Generate VAO
	void Init();
	// Bind shader & VAO, clean apply uniforms to shader, draw
	void Render();
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
	virtual void NotifyDirty(Cleanable* vertex_attribute) override;
	virtual void CleanObservees2() override;

	// Inherited: AttachingTo<VertexAttributeInterface>
	void Attach(VertexAttributeInterface* vertex_attribute) override;

	// Inherited: AttachingTo<Texture>
	void Attach(Texture* tx) override;

	VertexAttributeInterface* GetVertexAttribute(const std::string name);

	bool debug_ = false;

private:
	const ObjectType type_;
	const GLenum draw_mode_ = GL_STATIC_DRAW;

	std::unordered_map<std::string, VertexAttributeInterface*> vertex_attributes_;
	std::set<Cleanable*> vertex_attributes_dirty_;

	Texture* texture_ = nullptr;

	// Submit draw call
	void Draw() const;

	// Bind buffer and apply data to the given target location
	void BindData(GLenum target, GLenum buffer, GLsizeiptr size, const void* data);

	// Bind shader & VAO
	void BindObject() const;

	// Generate a buffer if it does not exist
	void GenBuffer(GLuint* id) const;

	const bool isIndexed() const;
	void TransformChanged();
	void ClearBuffers();
};

Object3D* LoadOBJ(const std::string path, const std::string filename, Shader* shader);

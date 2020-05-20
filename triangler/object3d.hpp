#pragma once

#include <map>
#include <typeindex>
#include <typeinfo>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "mesh.hpp"
#include "shader.hpp"

#define VERTEX_POS_ATTRIB_LOCATION 0
#define VERTEX_COLOR_ATTRIB_LOCATION 1
#define VERTEX_NORMAL_ATTRIB_LOCATION 2

struct Object3D
{
	GLuint vertex_array_id_ = -1;

	GLuint vertex_buffer_id = -1;
	GLuint index_buffer_id = -1;
	GLuint color_buffer_id = -1;
	GLuint normal_buffer_id = -1;

	Mesh mesh;

	Shader* shader_ = nullptr;
	std::map<std::string, ShaderUniformInterface*> uniforms_ = {
		{ "model_to_world", new ShaderUniform<glm::mat4>("model_to_world", glm::mat4(1.0f)) },
	};

	glm::mat4 transform_ = glm::mat4(1.0f);
	glm::mat4 transform_scale = glm::mat4(1.0f);
	glm::mat4 transform_rotation = glm::mat4(1.0f);
	glm::mat4 transform_translation = glm::mat4(1.0f);
	glm::vec4 line_color = glm::vec4(1.f);

	const GLenum draw_mode_ = GL_STATIC_DRAW;

	// Generate VAO
	void Init()
	{
		glUseProgram(shader_->id_);

		glGenVertexArrays(1, &vertex_array_id_);
		glBindVertexArray(vertex_array_id_);
	}

	void Draw() const
	{
		if(isIndexed()) glDrawArrays(GL_TRIANGLES, 0, mesh.v.size());
		else glDrawElements(GL_TRIANGLES, mesh.t.size(), GL_UNSIGNED_INT, (void*)0);
	}

	void Render() const
	{
		BindObject();
		ApplyUniforms();
		Draw();
	}

	// Bind shader & VAO
	void BindObject() const
	{
		glUseProgram(shader_->id_);
		glBindVertexArray(vertex_array_id_);
	}

	// Generate a buffer if it does not exist
	void GenBuffer(GLuint* id) const
	{
		if (glIsBuffer(*id) == GL_FALSE) glGenBuffers(1, id);
	}

	void BindData(GLenum target, GLenum buffer, GLsizeiptr size, const void* data)
	{
		glBindBuffer(target, buffer);
		glBufferData(target, size, data, draw_mode_);
	}

	// Add a mesh
	// Binds all vertex data in the mesh and generates colors & normals if missing
	void SetMesh(Mesh m)
	{
		if (m.v.empty()) return;

		mesh = m;
		BindObject();

		// Vertex pos
		{
			GenBuffer(&vertex_buffer_id);
			//BindData(GL_ARRAY_BUFFER, vertex_buffer_id, mesh.v.size() * sizeof(mesh.v[0]), mesh.v.data());
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
			glBufferData(GL_ARRAY_BUFFER, mesh.v.size() * sizeof(mesh.v[0]), mesh.v.data(), draw_mode_);

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
			glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
			glBufferData(GL_ARRAY_BUFFER, mesh.c.size() * sizeof(mesh.c[0]), mesh.c.data(), draw_mode_);

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
			glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
			glBufferData(GL_ARRAY_BUFFER, mesh.n.size() * sizeof(mesh.n[0]), mesh.n.data(), draw_mode_);

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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.t.size() * sizeof(mesh.t[0]), mesh.t.data(), draw_mode_);
		}
	}

	const bool isIndexed() const
	{
		return index_buffer_id == -1;
	}

	// Get origin in world-space
	glm::vec3 GetOrigin() const
	{
		return GetTransform() * glm::vec4(0);
	}

	const glm::vec3 GetMid() const
	{
		return GetTransform() * glm::vec4(mesh.mid, 1);
	}

	const glm::mat4 GetTransform() const
	{
		return transform_;
	}

	virtual void UpdateUniforms()
	{
		static_cast<ShaderUniform<glm::mat4>*>(uniforms_["model_to_world"])->SetValue(transform_);
	}

	void TransformChanged()
	{
		transform_ = transform_translation * transform_rotation * transform_scale;;
		UpdateUniforms();
	}

	void Translate(const glm::vec3 v)
	{
		transform_translation = glm::translate(transform_translation, v);
		TransformChanged();
	}

	void Translate(const glm::mat4 m)
	{
		transform_translation *= m;
		TransformChanged();
	}

	void Scale(const float factor)
	{
		transform_scale *= glm::scale(glm::vec3(factor));
		mesh.radius *= factor;
		TransformChanged();
	}

	void Rotate(const glm::mat4 m)
	{
		transform_rotation *= m;
		TransformChanged();
	}

	// Assumes global space axis
	void Rotate(const float deg, const glm::vec3 axis)
	{
		transform_rotation = glm::rotate(deg, axis) * transform_rotation;
		TransformChanged();
	}

	void ResetRotation()
	{
		transform_rotation = glm::mat4(1.f);
		TransformChanged();
	}

	void ResetScale()
	{
		transform_scale = glm::mat4(1.f);
		TransformChanged();
	}

	void ResetTranslation()
	{
		transform_translation = glm::mat4(1.f);
		TransformChanged();
	}

	void AddUniform(ShaderUniformInterface* uniform)
	{
		uniforms_.insert({ uniform->name_, uniform });
		uniform->Attach(shader_);
	}

	// Clean shader uniforms
	void ApplyUniforms() const
	{
		for (auto const& pair : uniforms_)
		{
			ShaderUniformInterface* uniform = pair.second;
			uniform->Clean(shader_);
		}
	}

	// Transform a direction vector to global space
	const glm::vec3 DirectionToGlobal(const glm::vec3 v) const
	{
		return glm::inverse(transform_rotation) * glm::inverse(transform_scale) * glm::vec4(v, 1.f);
	}

	void ClearBuffers()
	{
		glDeleteBuffers(1, &vertex_buffer_id);
		glDeleteBuffers(1, &index_buffer_id);
		glDeleteBuffers(1, &color_buffer_id);
		glDeleteBuffers(1, &normal_buffer_id);
		glDeleteVertexArrays(1, &vertex_array_id_);
	}

	//Object3D() {}
	Object3D(Shader* shader) : shader_(shader) {}
	Object3D(Shader* shader, Mesh m) : shader_(shader), mesh(m) {}
};

#include <chrono>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "object3d.hpp"
#include "vertexattribute.hpp"

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
	CleanObservees2(); // Rebind dirty vertex attributes
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

void Object3D::Attach(VertexAttributeInterface* vertex_attribute)
{
	// Generate buffers for attributes on attach
	BindObject();
	GenBuffer(&vertex_attribute->buffer_id_);
	vertex_attribute->AddObserver(this);

	vertex_attributes_.insert({ vertex_attribute->name_, vertex_attribute });
	NotifyDirty(vertex_attribute);
}

VertexAttributeInterface* Object3D::GetVertexAttribute(const std::string name)
{
	return vertex_attributes_[name];
}

void Object3D::NotifyDirty(Cleanable* vertex_attribute)
{
	vertex_attributes_dirty_.insert(vertex_attribute);
}

void Object3D::CleanObservees2()
{
	for (Cleanable* vertex_attribute : vertex_attributes_dirty_)
	{
		vertex_attribute->Clean();
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

	if (type_ == ObjectType::Mesh)
	{
		// Vertex normals
		if (mesh.n.empty()) genNormals(mesh);
		VertexAttribute<glm::vec3>* vertex_normals = new VertexAttribute<glm::vec3>(
			VERTEX_ATTRIB_NAME_NORMAL,
			VERTEX_ATTRIB_LOCATION_NORMAL,
			mesh.n
		);
		Attach(vertex_normals);

		// Texture coordinates
		if (!mesh.tx.empty())
		{
			VertexAttribute<glm::vec2>* tex_coords = new VertexAttribute<glm::vec2>(
				VERTEX_ATTRIB_NAME_TEXTURECOORD,
				VERTEX_ATTRIB_LOCATION_TEXTURECOORD,
				mesh.tx
				);
			Attach(tex_coords);
		}
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
	shader_->Attach(uniform);
}

// Clean shader uniforms
void Object3D::ApplyUniforms() const
{
	glUseProgram(shader_->id_);
	for (auto const& pair : uniforms_)
	{
		ShaderUniformInterface* uniform = pair.second;
		uniform->Clean();
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

void Object3D::Attach(Texture* tx)
{
	texture_ = tx;
}

// For identical vertex matching
struct Vertex
{
	size_t vx, vy, vz;
	size_t nx, ny, nz;
	size_t tu, tv;

	bool operator==(const Vertex& other) const
	{
		return (
			this->vx == other.vx &&
			this->vy == other.vy &&
			this->vz == other.vz &&
			this->nx == other.nx &&
			this->ny == other.ny &&
			this->nz == other.nz &&
			this->tu == other.tu &&
			this->tv == other.tv
			);
	}
};

// Hash function for Vertex struct (sum of powers of two)
namespace std {
	template<> struct hash<Vertex>
	{
		std::size_t operator()(const Vertex& v) const noexcept
		{
			return (
				(v.vx * v.vx) + (v.vy * v.vy) + (v.vz * v.vz) +
				(v.nx * v.nx) + (v.ny * v.ny) + (v.nz * v.nz) +
				(v.tu * v.tu) + (v.tv * v.tv)
				);
		}
	};
}

Object3D* LoadOBJ(const std::string filename, const std::string path, Shader* shader)
{
	// Start timer
	auto start = std::chrono::system_clock::now();

	// Load OBJ file
	Mesh m;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), path.c_str());

	if (!warn.empty()) std::cout << warn << std::endl;
	if (!err.empty()) std::cerr << err << std::endl;
	if (!loaded)
	{
		std::cerr << "Failed to load OBJ: " << path << std::endl;
		return nullptr;
	}
	if (shapes.empty())
	{
		std::cerr << "No shapes in OBJ: " << path << std::endl;
		return nullptr;
	}
	if (shapes[0].mesh.num_face_vertices.empty())
	{
		std::cerr << "No vertices in OBJ: " << path << std::endl;
		return nullptr;
	}
	if (shapes[0].mesh.num_face_vertices[0] != 3)
	{
		std::cerr << "Unsupported non-triangle primitives in OBJ: " << path << std::endl;
		return nullptr;
	}

	glm::dvec3 avg(0.0);
	unsigned offset = 0u;
	std::unordered_map<Vertex, int> previous;

	for (unsigned face = 0; face < shapes[0].mesh.num_face_vertices.size(); face++)
	{
		for (unsigned vertex = 0; vertex < 3; vertex++)
		{
			auto v_idx = shapes[0].mesh.indices[offset + vertex];
			Vertex v = {
				3 * v_idx.vertex_index + 0,
				3 * v_idx.vertex_index + 1,
				3 * v_idx.vertex_index + 2,
				3 * v_idx.normal_index + 0,
				3 * v_idx.normal_index + 1,
				3 * v_idx.normal_index + 2,
				2 * v_idx.texcoord_index + 0,
				2 * v_idx.texcoord_index + 1
			};

			auto match = previous.find(v);
			if (match == previous.end()) // New vertex
			{
				glm::vec3 vec = glm::vec3(
					attrib.vertices[v.vx],
					attrib.vertices[v.vy],
					attrib.vertices[v.vz]
				);
				m.v.emplace_back(vec);
				avg += vec;

				if (v_idx.normal_index != -1)
				{
					m.n.emplace_back(glm::vec3(
						attrib.normals[v.nx],
						attrib.normals[v.ny],
						attrib.normals[v.nz]
					));
				}

				if (v_idx.texcoord_index != -1)
				{
					m.tx.emplace_back(glm::vec2(
						attrib.texcoords[v.tu],
						attrib.texcoords[v.tv]
					));
				}
				size_t index = previous.size();
				previous.insert({ v, index });
				m.t.emplace_back(index);
			}
			else
			{
				m.t.emplace_back(match->second);
			}
		}
		offset += 3;
	}

	float maxlen = 0;
	avg = avg / static_cast<double>(m.v.size());
	for (auto& vert : m.v)
	{
		// Center to mid
		vert = (vert - glm::vec3(avg));
		// Find furthest vertice (mesh radius)
		if (glm::length2(vert) > maxlen) maxlen = glm::length2(vert);
	}
	m.radius = sqrtf(maxlen);
	m.mid = glm::vec3(0.f);

	std::cout << "Materials: " << materials.size() << std::endl;

	for (auto m : materials)
	{
		std::cout
			<< "Material '" << m.name << "':" << std::endl
			<< "  ambient_texname: " << m.ambient_texname << std::endl
			<< "  diffuse_texname: " << m.diffuse_texname << std::endl
			<< "  specular_texname: " << m.specular_texname << std::endl
			<< "  ambient: " << m.ambient << std::endl;
	}

	Object3D* obj = new Object3D(ObjectType::Mesh, shader);
	obj->Init();
	obj->SetMesh(m);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout
		<< "Loaded OBJ mesh, vertices: " << m.v.size()
		<< ", triangles: " << (float)m.t.size() / 3.f
		<< ", normals: " << m.n.size()
		//<< ", texcoords: " << m.tx.size()
		<< ", colors: " << m.c.size()
		<< ", time: " << elapsed_seconds.count() << "s"
		<< std::endl;

	return obj;
}

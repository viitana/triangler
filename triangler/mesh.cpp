#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/gtx/norm.hpp>
#include <boost/lexical_cast.hpp>

#include "mesh.hpp"

void genNormals(Mesh& mesh)
{
	std::vector<glm::vec3> normals(mesh.v.size());

	for (int i = 0; i < mesh.t.size(); i += 3)
	{
		auto v0 = mesh.v[mesh.t[i + 0]];
		auto v1 = mesh.v[mesh.t[i + 1]];
		auto v2 = mesh.v[mesh.t[i + 2]];

		auto n = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		normals[mesh.t[i + 0]] += n;
		normals[mesh.t[i + 1]] += n;
		normals[mesh.t[i + 2]] += n;
	}

	for (auto& normal : normals)
	{
		normal = glm::normalize(normal);
	}

	mesh.n = normals;
}

void genRandomColors(Mesh& mesh) {
	for (int i = 0; i < mesh.v.size(); i++) {
		mesh.AddColor(glm::vec4(
			(float)rand() / (float)RAND_MAX,
			(float)rand() / (float)RAND_MAX,
			(float)rand() / (float)RAND_MAX,
			1.0f
		));
	}
}

Mesh genIcosahedron()
{
	Mesh mesh;

	const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

	mesh.AddVert(glm::vec3(-1.0f, t, 0.0f));
	mesh.AddVert(glm::vec3(1.0f, t, 0.0f));
	mesh.AddVert(glm::vec3(-1.0f, -t, 0.0f));
	mesh.AddVert(glm::vec3(1.0f, -t, 0.0f));
	mesh.AddVert(glm::vec3(0.0f, -1.0, t));
	mesh.AddVert(glm::vec3(0.0f, 1.0, t));
	mesh.AddVert(glm::vec3(0.0f, -1.0, -t));
	mesh.AddVert(glm::vec3(0.0f, 1.0, -t));
	mesh.AddVert(glm::vec3(t, 0.0f, -1.0f));
	mesh.AddVert(glm::vec3(t, 0.0f, 1.0f));
	mesh.AddVert(glm::vec3(-t, 0.0f, -1.0f));
	mesh.AddVert(glm::vec3(-t, 0.0f, 1.0f));

	mesh.AddTri(0, 11, 5);
	mesh.AddTri(0, 5, 1);
	mesh.AddTri(0, 1, 7);
	mesh.AddTri(0, 7, 10);
	mesh.AddTri(0, 10, 11);
	mesh.AddTri(1, 5, 9);
	mesh.AddTri(5, 11, 4);
	mesh.AddTri(11, 10, 2);
	mesh.AddTri(10, 7, 6);
	mesh.AddTri(7, 1, 8);
	mesh.AddTri(3, 9, 4);
	mesh.AddTri(3, 4, 2);
	mesh.AddTri(3, 2, 6);
	mesh.AddTri(3, 6, 8);
	mesh.AddTri(3, 8, 9);
	mesh.AddTri(4, 9, 5);
	mesh.AddTri(2, 4, 11);
	mesh.AddTri(6, 2, 10);
	mesh.AddTri(8, 6, 7);
	mesh.AddTri(9, 8, 1);

	mesh.Normalize();
	mesh.radius = 1.0f;

	return mesh;
}

struct Edge
{
	int v0;
	int v1;

	Edge(int v0, int v1)
		: v0(v0 < v1 ? v0 : v1), v1(v0 < v1 ? v1 : v0) {}

	bool operator <(const Edge& rhs) const
	{
		return v0 < rhs.v0 || (v0 == rhs.v0 && v1 < rhs.v1);
	}
};

int subdivideEdge(int f0, int f1, const glm::vec3& v0, const glm::vec3& v1, Mesh& io_mesh, std::map<Edge, int>& io_divisions)
{
	const Edge edge(f0, f1);
	auto it = io_divisions.find(edge);
	if (it != io_divisions.end())
	{
		return it->second;
	}

	const glm::vec3 v = normalize((v0 + v1) * 0.5f);
	const int f = io_mesh.v.size();
	io_mesh.v.emplace_back(v);
	io_divisions.emplace(edge, f);
	return f;
}

void SubdivideMesh(const Mesh& meshIn, Mesh& meshOut)
{
	meshOut.v = meshIn.v;

	std::map<Edge, int> divisions; // Edge -> new vertex

	for (uint32_t i = 0; i < meshIn.t.size() / 3; ++i)
	{
		const int f0 = meshIn.t[i * 3];
		const int f1 = meshIn.t[i * 3 + 1];
		const int f2 = meshIn.t[i * 3 + 2];

		const glm::vec3 v0 = meshIn.v[f0];
		const glm::vec3 v1 = meshIn.v[f1];
		const glm::vec3 v2 = meshIn.v[f2];

		const int f3 = subdivideEdge(f0, f1, v0, v1, meshOut, divisions);
		const int f4 = subdivideEdge(f1, f2, v1, v2, meshOut, divisions);
		const int f5 = subdivideEdge(f2, f0, v2, v0, meshOut, divisions);

		meshOut.AddTri(f0, f3, f5);
		meshOut.AddTri(f3, f1, f4);
		meshOut.AddTri(f4, f2, f5);
		meshOut.AddTri(f3, f4, f5);
	}
}

Mesh genIcosphere(int subdivisions)
{
	Mesh m = genIcosahedron();
	Mesh m2;

	for (int i = 0; i < subdivisions; i++)
	{
		SubdivideMesh (m, m2);
		m.v = m2.v;
		m.t = m2.t;
		m2.Clear();
	}
	return m;
}

Mesh genRing(const int points, const glm::vec4 color)
{
	Mesh m;
	for (int i = 0; i < points; i++)
	{
		m.AddVert({ std::cosf(2.f * PI * ((float)i / points)), 0.f, std::sinf(2 * PI * ((float)i / points)) });
		m.AddVert({ std::cosf(2.f * PI * (((float)i + 1) / points)), 0.f, std::sinf(2 * PI * (((float)i + 1) / points)) });
		m.AddColor(color);
		m.AddColor(color);
	}
	m.radius = 1.f;

	return m;
}

void genGrid(Mesh& mesh, const float dim, const int segments, const int subgrids, const glm::vec4 color)
{
	float start = -0.5f * dim;
	float pos = start;
	const float step = dim / segments;
	for (int i = 0; i <= segments; i++)
	{
		mesh.AddVert(glm::vec3(start, 0, pos));
		mesh.AddVert(glm::vec3(-start, 0, pos));
		mesh.AddVert(glm::vec3(pos, 0, start));
		mesh.AddVert(glm::vec3(pos, 0, -start));
		pos += step;

		for (int j = 0; j < 4; j++)
			mesh.AddColor(color);
	}

	if (subgrids > 0)
		genGrid(mesh, dim, segments * 10, subgrids - 1, glm::vec4(color[0], color[1], color[2], 0.5f * color[3]));
}

Mesh genGrid(const float dim, const int segments, const int subgrids, const glm::vec4 color)
{
	Mesh m;
	genGrid(m, dim, segments, subgrids, color);

	if (subgrids > 0)
	{
		genGrid(m,
			dim,
			segments * 10,
			subgrids - 1,
			glm::vec4(color[0], color[1], color[2], 0.5f * color[3])
		);
	}
	return m;
}

Mesh genVector(const glm::vec3 start, const glm::vec3 end, const glm::vec4 color)
{
	Mesh m;
	m.AddVert(start);
	m.AddVert(end);
	m.AddColor(color);
	m.AddColor(color);
	return m;
}

// seek till digit, sign or eof
// signless for int
void seekNextNumber(std::istringstream& iss)
{
	while (!std::isdigit(iss.peek()) && iss.peek() != '-' && iss.peek() != EOF)
		iss.ignore(1, EOF);
}

bool extractFloats(std::istringstream& iss, float* coord)
{
	for (int i = 0; i < 3; i++)
	{
		seekNextNumber(iss);

		char next = iss.peek();
		std::string s;
		std::getline(iss, s, ' ');
		try
		{
			coord[i] = boost::lexical_cast<float>(s);
		}
		catch (boost::bad_lexical_cast&)
		{
			std::cout << "Bad cast: " << s << " : '" << next << "', eof: " << (next == EOF) << std::endl;
		}
	}

	return true;
}

bool getInt(std::istringstream& iss, std::string& s)
{
	// Look ahead til we get to end of integer
	auto start = iss.tellg();
	while (std::isdigit(iss.peek()))
		iss.ignore(1, EOF);

	char end = iss.peek();

	// Seek back
	iss.clear();
	iss.seekg(start);
	std::getline(iss, s, end);

	// Seek till digit, space or eof
	while (!std::isdigit(iss.peek()) && !std::isspace(iss.peek()) && iss.peek() != EOF)
		iss.ignore(1, EOF);

	// Return whether next is a slash
	return end == '/';
}

bool extractInts(std::istringstream& iss, float* values)
{
	std::string s;
	bool proceed;

	for (int i = 0; i < 3; i++)
	{
		seekNextNumber(iss);
		proceed = true;
		if (proceed)
		{
			proceed = getInt(iss, s);
			values[i + 0] = boost::lexical_cast<float>(s);
		}
		if (proceed)
		{
			proceed = getInt(iss, s);
			values[i + 3] = boost::lexical_cast<float>(s);
		}
		if (proceed)
		{
			proceed = getInt(iss, s);
			values[i + 6] = boost::lexical_cast<float>(s);
		}
	}

	return true;
}

Mesh LoadOBJ(const std::string path, const glm::vec3 offset)
{
	Mesh m;

	auto start = std::chrono::system_clock::now();
	std::ifstream infile(path);

	int lineCount = 0;
	int lineTotal = std::count(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>(), '\n');

	infile.seekg(0, std::ios::beg);

	std::string line;
	glm::dvec3 avg(0.0);

	int v = 0;
	int t = 0;

	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		if (iss.peek() == 'v')
		{
			float coord[3];
			iss.ignore(1, EOF);

			if (iss.peek() == 'n')
			{
				extractFloats(iss, coord);
				// TODO: vertex normal support
			}
			else
			{
				extractFloats(iss, coord);
				glm::vec3 vec = glm::vec3(coord[0], coord[1], coord[2]);
				m.AddVert(vec);
				avg += vec;
				v++;
			}
		}
		else if (iss.peek() == 'f')
		{
			float values[9];
			extractInts(iss, values);
			m.AddTri(values[0] - 1, values[1] - 1, values[2] - 1);
			t++;
		}
	}

	avg = avg / static_cast<double>(v);

	glm::vec3 avgVec = glm::vec3(avg);
	float maxlen = 0;
	for (auto& vert : m.v)
	{
		// Center to mid
		vert = (vert - avgVec);
		// Find furthest vertice (mesh radius)
		if (glm::length2(vert) > maxlen) maxlen = glm::length2(vert);
	}

	m.radius = sqrtf(maxlen);
	m.mid = glm::vec3(0.f);// avgVec;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout
		<< "Loaded OBJ mesh, vertices: "
		<< m.v.size() << "/" << v
		<< ", triangles: "
		<< m.t.size() / 3 << "/" << t
		<< ", time: "
		<< elapsed_seconds.count() << "s"
		<< std::endl;

	return m;
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
			+this->tu == other.tu &&
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


Mesh LoadOBJFast(const std::string filename, const std::string path)
{
	auto start = std::chrono::system_clock::now();

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	Mesh m;
	glm::dvec3 avg(0.0);

	bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), path.c_str());

	if (!warn.empty()) std::cout << warn << std::endl;
	if (!err.empty()) std::cerr << err << std::endl;
	if (!loaded)
	{
		std::cerr << "Failed to load OBJ: " << path << std::endl;
		return m;
	}
	if (shapes.empty())
	{
		std::cerr << "No shapes in OBJ: " << path << std::endl;
		return m;
	}
	if (shapes[0].mesh.num_face_vertices.empty())
	{
		std::cerr << "No vertices in OBJ: " << path << std::endl;
		return m;
	}
	if (shapes[0].mesh.num_face_vertices[0] != 3)
	{
		std::cerr << "Unsupported non-triangle primitives in OBJ: " << path << std::endl;
		return m;
	}

	unsigned idx = 0u;
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

				//if (v_idx.texcoord_index != -1)
				//{
				//	m.tx.emplace_back(glm::vec2(
				//		attrib.texcoords[v.tu],
				//		attrib.texcoords[v.tv]
				//	));
				//}
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

	return m;
}

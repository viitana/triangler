#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

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

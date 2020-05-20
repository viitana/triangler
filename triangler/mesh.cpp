#include <map>

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

struct EdgeM
{
	int v0;
	int v1;

	EdgeM(int v0, int v1)
		: v0(v0 < v1 ? v0 : v1), v1(v0 < v1 ? v1 : v0) {}

	bool operator <(const EdgeM& rhs) const
	{
		return v0 < rhs.v0 || (v0 == rhs.v0 && v1 < rhs.v1);
	}
};

int subdivideEdgeM(int f0, int f1, const glm::vec3& v0, const glm::vec3& v1, Mesh& io_mesh, std::map<EdgeM, int>& io_divisions)
{
	const EdgeM edge(f0, f1);
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

void SubdivideMeshM(const Mesh& meshIn, Mesh& meshOut)
{
	meshOut.v = meshIn.v;

	std::map<EdgeM, int> divisions; // Edge -> new vertex

	for (uint32_t i = 0; i < meshIn.t.size() / 3; ++i)
	{
		const int f0 = meshIn.t[i * 3];
		const int f1 = meshIn.t[i * 3 + 1];
		const int f2 = meshIn.t[i * 3 + 2];

		const glm::vec3 v0 = meshIn.v[f0];
		const glm::vec3 v1 = meshIn.v[f1];
		const glm::vec3 v2 = meshIn.v[f2];

		const int f3 = subdivideEdgeM(f0, f1, v0, v1, meshOut, divisions);
		const int f4 = subdivideEdgeM(f1, f2, v1, v2, meshOut, divisions);
		const int f5 = subdivideEdgeM(f2, f0, v2, v0, meshOut, divisions);

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
		SubdivideMeshM (m, m2);
		m.v = m2.v;
		m.t = m2.t;
		m2.Clear();
	}
	return m;
}

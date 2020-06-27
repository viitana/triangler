#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <string>

#define PI 3.1415927f

struct Mesh
{
	// Index
	std::vector<int> t;
	// Vertices
	std::vector<glm::vec3> v;
	// Normals
	std::vector<glm::vec3> n;
	// Colors
	std::vector<glm::vec4> c;
	// Texture coordinates
	std::vector <glm::vec2> tx;

	// Mid point
	glm::vec3 mid = { 0, 0 , 0 };

	float radius;

	void AddVert(glm::vec3 vert)
	{
		v.emplace_back(vert);
	}

	void AddColor(glm::vec4 vert)
	{
		c.emplace_back(vert);
	}

	void AddTri(int a, int b, int c)
	{
		t.emplace_back(a);
		t.emplace_back(b);
		t.emplace_back(c);
	}

	void Clear() {
		v.clear();
		t.clear();
	}

	void Normalize()
	{
		for (auto& vert : v)
			vert = glm::normalize(vert);
	}
};

void genNormals(Mesh& mesh);
void genRandomColors(Mesh& mesh);

Mesh genIcosahedron();
Mesh genIcosphere(const int subdivisions);
Mesh genRing(const int points, const glm::vec4 color);
Mesh genGrid(const float dim, const int segments, const int subgrids, const glm::vec4 color);
Mesh genVector(const glm::vec3 start, const glm::vec3 end, const glm::vec4 color);

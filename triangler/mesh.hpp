#pragma once

#include <glm/glm.hpp>

#include <vector>

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
Mesh genIcosphere(int subdivisions);
Mesh genIcosahedron();

#pragma once

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

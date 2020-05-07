#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <regex> 
#include <chrono>
#include <ctime>   
#include <limits>
#include <glm/gtx/norm.hpp>

#include <glm/gtx/component_wise.hpp>
#include <boost/lexical_cast.hpp>

#include "util.hpp"

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

void addDebugVector(Mesh& mesh, const glm::vec3 start, const glm::vec3 end, const glm::vec4 color)
{
	mesh.v.insert(mesh.v.begin(), start);
	mesh.v.insert(mesh.v.begin(), end);
	mesh.c.insert(mesh.c.begin(), color);
	mesh.c.insert(mesh.c.begin(), color);
}

const std::vector<glm::vec3> genCube() {
	return {
		{-1.0f,-1.0f,-1.0f},
		{-1.0f,-1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f,-1.0f},
		{-1.0f, -1.0f, -1.0f},
		{ -1.0f, 1.0f,-1.0f },
		{ 1.0f,-1.0f, 1.0f },
		{-1.0f, -1.0f, -1.0f},
		{ 1.0f,-1.0f,-1.0f },
		{1.0f, 1.0f,-1.0f},
		{1.0f,-1.0f,-1.0f},
		{-1.0f,-1.0f,-1.0f},
		{-1.0f,-1.0f,-1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f,-1.0f},
		{1.0f,-1.0f, 1.0f},
		{-1.0f,-1.0f, 1.0f},
		{-1.0f,-1.0f,-1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f,-1.0f, 1.0f},
		{1.0f,-1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f,-1.0f,-1.0f},
		{1.0f, 1.0f,-1.0f},
		{1.0f,-1.0f,-1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f,-1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f,-1.0f},
		{-1.0f, 1.0f,-1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f,-1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f,-1.0f, 1.0f}
	};

}

void genIcosahedron(Mesh& mesh)
{
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

void genIcosphere(Mesh& mesh, int subdivisions)
{
	Mesh m;
	genIcosahedron(mesh);
	for (int i = 0; i < subdivisions; i++)
	{
		SubdivideMesh(mesh, m);
		mesh.v = m.v;
		mesh.t = m.t;
		m.Clear();
	}
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

void LoadOBJf(Object3D* obj, std::string path, glm::vec3 offset)
{
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
				obj->mesh.AddVert(vec);
				avg += vec;
				v++;
			}
		}
		else if (iss.peek() == 'f')
		{
			float values[9];
			extractInts(iss, values);
			obj->mesh.AddTri(values[0] - 1, values[1] - 1, values[2] - 1);
			t++;
		}
	}

	avg = avg / static_cast<double>(v);

	glm::vec3 avgVec = glm::vec3(avg);
	float maxlen = 0;
	for (auto& vert : obj->mesh.v)
	{
		// Center to mid
		vert = (vert - avgVec);
		// Find furthest vertice (mesh radius)
		if (glm::length2(vert) > maxlen) maxlen = glm::length2(vert);
	}

	obj->mesh.radius = sqrtf(maxlen);
	obj->mesh.mid = glm::vec3(0.f);// avgVec;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout
		<< "Loaded OBJ mesh, vertices: "
		<< obj->mesh.v.size() << "/" << v
		<< ", triangles: "
		<< obj->mesh.t.size() / 3 << "/" << t
		<< ", time: "
		<< elapsed_seconds.count() << "s"
		<< std::endl;

}

void printVec(glm::vec3 v)
{
	std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
}

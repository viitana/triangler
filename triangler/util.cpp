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

void printVec(glm::vec3 v)
{
	std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
}

// Relative to circle middle
const glm::vec3 ClosestCirlePoint(const glm::vec3 mid, const float r, const glm::vec3 p)
{
	return mid + r * glm::normalize(p - mid);
}

#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "mesh.hpp"

class Ray
{
public:
	Ray(glm::vec3 origin, glm::vec3 direction);
	const bool Intersect(Mesh& mesh, float& t);
	const bool IntersectXZPlane(float y, float& t);
	const bool IntersectPlane(const glm::vec3 p, const glm::vec3 n, float& t);
	const bool IntersectSphere(const glm::vec3 mid, const float r, float& t);
	const bool IntersectRing(const glm::vec3 p, const glm::vec3 n, const float r, float& t);
	const glm::vec3 Origin() { return origin_; }
	const glm::vec3 Direction() { return direction_; }
private:
	glm::vec3 origin_;
	glm::vec3 direction_;
};

#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "mesh.hpp"

class Ray
{
public:
	Ray(glm::vec3 origin, glm::vec3 direction);
	void Intersect(Mesh& mesh, float& t);
	void IntersectXZPlane(float y, float& t);
	void IntersectSphere(const glm::vec3 mid, const float r, float& t);
	const glm::vec3 Origin() { return origin_; }
	const glm::vec3 Direction() { return direction_; }
private:
	glm::vec3 origin_;
	glm::vec3 direction_;
};

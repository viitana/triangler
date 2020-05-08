#include <iostream>

#include "ray.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : origin_(origin), direction_(direction) {}

const bool Ray::Intersect(Mesh& mesh, float& t)
{
	return false;
}

const bool Ray::IntersectXZPlane(float y, float& t)
{
	t = -(y + glm::dot(glm::vec3(0, 1, 0), origin_)) / glm::dot(glm::vec3(0, 1, 0), direction_);
	return t > 0;
}

const bool Ray::IntersectPlane(const glm::vec3 p, const glm::vec3 n, float& t)
{
	const float d = -n.x * p.x - n.y * p.y - n.z * p.z;
	t = -(d + glm::dot(n, origin_)) / glm::dot(n, direction_);
	return t > 0;
}

const bool Ray::IntersectRing(const glm::vec3 p, const glm::vec3 n, const float r, float& t)
{
	const bool planehit = IntersectPlane(p, n, t);
	if (!planehit) return false;
	return std::abs(glm::distance(p, origin_ + t * direction_) - r) < 0.05f * r;
}

const bool Ray::IntersectSphere(const glm::vec3 mid, const float r, float& t)
{
	auto originAdjusted = origin_ - mid;

	const float a = glm::dot(direction_, direction_);
	const float b = 2.0f * glm::dot(direction_, originAdjusted);
	const float c = glm::dot(originAdjusted, originAdjusted) - r * r;

	const float disc = b * b - 4.0f * a * c;
	
	if (disc < 0) return false;

	const float t1 = (-b + sqrtf(disc)) / (2 * a);
	const float t2 = (-b - sqrtf(disc)) / (2 * a);

	// Return closest positive result
	if (t1 < t2)
	{
		if (t1 < 0)
			t = t2;
		else
			t = t1;
	}
	else
	{
		if (t2 < 0)
			t = t1;
		else
			t = t2;
	}
	return true;
}

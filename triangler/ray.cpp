#include "ray.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : origin_(origin), direction_(direction) {}

void Ray::Intersect(Mesh& mesh, float& t)
{

}

void Ray::IntersectXZPlane(float y, float& t)
{
	t = -(y + glm::dot(glm::vec3(0, 1, 0), origin_)) / glm::dot(glm::vec3(0, 1, 0), direction_);
}

void IntersectSphere(const glm::vec3 mid, const float r, float& t)
{

}

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "camera.hpp"

Camera::Camera(glm::vec3 camera_pos, glm::vec3 camera_dir, float fov, float fnear, float ffar)
	: camera_pos_(camera_pos), camera_dir_(camera_dir), fov_(fov), fnear_(fnear), ffar_(ffar) {}

void Camera::Rotate(float angle, glm::vec3 vec)
{
	camera_dir_ = glm::rotate(camera_dir_, angle, vec);
}

void Camera::Move(const glm::vec3 v)
{
	camera_pos_ += v;
}

void Camera::SetPosition(const glm::vec3 pos)
{
	camera_pos_ = pos;
}

const glm::mat4 Camera::Projection(const float width, const float height)
{
	size_ = glm::ivec2(width, height);
	w_ = glm::normalize(camera_dir_);
	u_ = glm::normalize(glm::cross(w_, up_));
	v_ = glm::normalize(glm::cross(u_, w_));

	return glm::perspective(fov_, (float)width / (float)height, fnear_, ffar_);
}

const glm::mat4 Camera::View() const
{
	return glm::lookAt(camera_pos_, camera_pos_ + camera_dir_, up_);
}

const Ray Camera::GenerateRay(const float pixelx, const float pixely) const
{
	const float nx = -2.0f * ((size_[0] - pixelx) / size_[0]) + 1.0f;
	const float ny = 2.0f * ((size_[1] - pixely) / size_[1]) - 1.0f;

	const float d = 1.f / tanf(fov_ / 2);
	const float aspect = (float)size_[0] / (float)size_[1];

	return Ray(camera_pos_, glm::normalize(aspect * nx * u_ + ny * v_ + d * w_));
}

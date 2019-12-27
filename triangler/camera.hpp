#pragma once

#include <glm/glm.hpp>

#include "ray.hpp"

class Camera
{
public:
	Camera(glm::vec3 camera_pos, glm::vec3 camera_dir, float fov, float fnear, float ffar);
	void Rotate(float angle, glm::vec3 vec);
	void RotateX(float angle) { Rotate(angle, up_); }
	void RotateY(float angle) { Rotate(angle, u_); }
	void Move(const glm::vec3 v);
	void MoveX(float dist) { Move(u_ * dist); }
	void MoveZ(float dist) { Move(w_ * dist); }
	void SetPosition(const glm::vec3 pos);
	const glm::mat4 Projection(const float width, const float height);
	const glm::mat4 View() const;
	const Ray GenerateRay(const float pixelx, const float pixely) const;
	const glm::vec3 Position() { return camera_pos_; }
private:
	glm::vec3 camera_pos_;
	glm::vec3 camera_dir_;
	glm::vec3 up_ = { 0, 1, 0 };

	float fov_;
	float fnear_;
	float ffar_;

	glm::ivec2 size_;
	glm::vec3 u_;
	glm::vec3 v_;
	glm::vec3 w_;
};

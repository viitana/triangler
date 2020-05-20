#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "object3d.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "mesh.hpp"

struct ObjectFocuser
{
	// Current object in focus
	Object3D* focused_obj_ = nullptr;

	std::vector<glm::vec3> standard_base_ = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
	};

	// Axis rings and points
	std::vector<Object3D*> axis_rings_;
	Object3D* axis_points_;

	// Default colors
	std::vector<glm::vec4> axis_colors_ = {
		{1, 0, 0, 1},
		{0, 1, 0, 1},
		{0, 0, 1, 1},
	};
	glm::vec4 axis_ring_highlight_color_ = { 1, 1, 1, 1 };

	void FocusObject(Object3D* obj)
	{
		// Move coordinate rings to enclose focused object
		for (Object3D* r : axis_rings_)
		{
			r->ResetTranslation();
			r->Translate(obj->GetMid());
			r->ResetScale();
			r->Scale(obj->mesh.radius);
		}
		focused_obj_ = obj;
	}

	void UpdateRing(Ray r, const unsigned n)
	{
		float rt;
		const bool hit = r.IntersectRing(
			axis_rings_[n]->GetMid(),
			standard_base_[n],
			focused_obj_->mesh.radius,
			rt
		);

		axis_rings_[n]->line_color = hit ? axis_ring_highlight_color_ : axis_colors_[n];
		axis_points_->mesh.v[n] = ClosestCirlePoint(
			axis_rings_[n]->GetMid(),
			focused_obj_->mesh.radius,
			r.Origin() + rt * r.Direction()
		);
		axis_points_->mesh.c[n] = hit ? axis_colors_[n] : glm::vec4(0.f);
	}

	void UpdateIndicators(const Ray r)
	{
		for (unsigned u = 0; u < axis_rings_.size(); u++)
		{
			UpdateRing(r, u);
		}
	}

	ObjectFocuser(Shader* shader_line, Shader* shader_point)
	{
		for (unsigned u = 0u; u < 3u; u++)
		{
			Object3D* ring = new Object3D(shader_line);
			genRing(ring, 64, axis_colors_[u]); // TODO: remove
			axis_rings_.push_back(ring);
			// init?
		}

		axis_points_ = new Object3D(shader_point, Mesh{
			{},
			{{ 0, 0, 0}, { 0, 0, 0 }, { 0, 0, 0 }},
			{},
			{{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }},
		});

		// Rotate x, z axis rings to correct orientations
		axis_rings_[0]->Rotate(PI / 2.f, { 0, 0, 1 });
		axis_rings_[2]->Rotate(PI / 2.f, { 1, 0, 0 });
	}
};

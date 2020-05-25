#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "object3d.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "mesh.hpp"

#define RING_SEGMENT_COUNT 64

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
	std::vector<std::vector<glm::vec4>> axis_colors_ = {
		std::vector<glm::vec4>(RING_SEGMENT_COUNT * 2, {1, 0, 0, 1}),
		std::vector<glm::vec4>(RING_SEGMENT_COUNT * 2, {0, 1, 0, 1}),
		std::vector<glm::vec4>(RING_SEGMENT_COUNT * 2, {0, 0, 1, 1}),
	};
	std::vector<glm::vec4> axis_ring_highlight_color_
		= std::vector<glm::vec4>(RING_SEGMENT_COUNT * 2, { 1, 1, 1, 1 });

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

	void UpdateIndicators(Ray r)
	{
		Mesh pmesh;

		for (unsigned ring = 0; ring < axis_rings_.size(); ring++)
		{
			float rt;
			const bool hit = r.IntersectRing(
				axis_rings_[ring]->GetMid(),
				standard_base_[ring],
				focused_obj_->mesh.radius,
				rt
			);

			static_cast<VertexAttribute<glm::vec4>*>(
				axis_rings_[ring]->GetVertexAttribute(VERTEX_ATTRIB_NAME_COLOR))
				->SetData(hit ? axis_ring_highlight_color_ : axis_colors_[ring]);

			pmesh.AddVert(ClosestCirlePoint(
				axis_rings_[ring]->GetMid(),
				focused_obj_->mesh.radius,
				r.Origin() + rt * r.Direction()
			));
			pmesh.AddColor(hit ? axis_colors_[ring][0] : glm::vec4(0.f));
		}

		static_cast<VertexAttribute<glm::vec3>*>(
			axis_points_->GetVertexAttribute(VERTEX_ATTRIB_NAME_POSITION))
			->SetData(pmesh.v);

		static_cast<VertexAttribute<glm::vec4>*>(
			axis_points_->GetVertexAttribute(VERTEX_ATTRIB_NAME_COLOR))
			->SetData(pmesh.c);

	}

	ObjectFocuser(Shader* shader_line, Shader* shader_point)
	{
		for (unsigned u = 0u; u < 3u; u++)
		{
			Object3D* ring = new Object3D(ObjectType::Line, shader_line);
			ring->Init();
			ring->SetMesh(genRing(RING_SEGMENT_COUNT, axis_colors_[u][0]));
			axis_rings_.push_back(ring);
		}

		axis_points_ = new Object3D(ObjectType::Point, shader_point);
		axis_points_->Init();
		axis_points_->SetMesh(Mesh{
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

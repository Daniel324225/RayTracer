#pragma once

#include "Vec3.h"

struct MaterialIndex
{
	size_t type_index;
	size_t vector_index;
};

struct HitRecord {
	Position position;
	Direction normal;
	MaterialIndex material;
	double t{};
	bool front_face{};

	void set_face_normal(const Direction& ray_direction, const Direction& outward_normal) {
		front_face = ray_direction.dot(outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};
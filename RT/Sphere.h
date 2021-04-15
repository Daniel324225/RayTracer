#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "HitRecord.h"
#include "Material.h"
#include <optional>
#include <cmath>

class Sphere {
	Position center_;
	double radius_;
	MaterialIndex material_;

public:
	Sphere(const Position& center, double radius, const MaterialIndex& material) : center_{ center }, radius_{ radius }, material_{material} {}

	[[nodiscard]]
	std::optional<HitRecord> intersect(const Ray& ray, double t_min, double t_max) const noexcept {
		const auto oc = ray.origin() - center_;
		const auto a = ray.direction().length_squared();
		const auto half_b = oc.dot(ray.direction());
		const auto c = oc.length_squared() - radius_ * radius_;

		const auto discriminant = half_b * half_b - a * c;

		if (discriminant < 0) return {};

		const auto sqrt_d = std::sqrt(discriminant);

		auto root = (-half_b - sqrt_d) / a;
		if (root < t_min || t_max < root) {
			root = (-half_b + sqrt_d) / a;
			if (root < t_min || t_max < root) {
				return {};
			}
		}

		return [&] {
			HitRecord r;
			r.position = ray.at(root);
			r.material = material_;
			r.t = root;
			r.set_face_normal(ray.direction(), (r.position - center_) / radius_);

			return r;
		}();
	}
};

static_assert(Hittable<Sphere>);
#pragma once
#include "Ray.h"
#include "HitRecord.h"
#include <optional>

template <typename T>
concept Hittable = requires (const T a, const Ray r) {
	{ a.intersect(r, double{}, double{}) } -> std::same_as<std::optional<HitRecord>>;
};
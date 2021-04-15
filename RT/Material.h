#pragma once

#include "Ray.h"
#include "Vec3.h"
#include "HitRecord.h"

#include <optional>


struct ScatterResult
{
	Color attenuation;
	Ray scattered;
};

template <typename T>
concept Material = requires (const T m, const Ray ray, const HitRecord& hr) {
	{ m.scatter(ray, hr) } -> std::same_as<std::optional<ScatterResult>>;
};

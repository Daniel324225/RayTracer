#pragma once

#include "Material.h"
#include "Vec3.h"

#include <optional>

class Lambertian {
	Color color;

public:
	Lambertian(const Color& color) : color{ color } {}

	[[nodiscard]]
	std::optional<ScatterResult> scatter(const Ray&, const HitRecord& hr) const noexcept {
		auto direction = hr.normal + Vec3::random_unit();

		auto near_zero = [](const Vec3& v) {
			for (auto s : v.data) {
				if (std::abs(s) > std::numeric_limits<double>::epsilon()) return false;
			}
			return true;
		};

		if (near_zero(direction)) direction = hr.normal;

		return ScatterResult{
			color,
			{hr.position, direction}
		};
	}
};

static_assert(Material<Lambertian>);

class Metal {
	Color color;
	double fuzz;

public:
	Metal(const Color& color, double fuzz) : color{ color }, fuzz{fuzz} {}

	[[nodiscard]]
	std::optional<ScatterResult> scatter(const Ray& ray, const HitRecord& hr) const noexcept {
		auto reflected = ray.direction().reflected(hr.normal).unit() + Vec3::random_in_sphere(fuzz);
		if (hr.normal.dot(reflected) > 0) {
			return ScatterResult{
				color,
				{hr.position, reflected}
			};
		}
		return {};
	}
};

static_assert(Material<Metal>);

class Dielectric {
	double index_of_refraction;

	[[nodiscard]]
	static double reflectance(double cosine, double ref_idx) noexcept {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
public:
	Dielectric(double index_of_refraction) : index_of_refraction{ index_of_refraction } {}

	[[nodiscard]]
	std::optional<ScatterResult> scatter(const Ray& ray, const HitRecord& hr) const noexcept {
		const auto refraction_ratio = hr.front_face ? (1.0 / index_of_refraction) : index_of_refraction;

		const auto unit_direction = ray.direction().unit();

		const auto cos_theta = std::min((-unit_direction).dot(hr.normal), 1.0);
		const auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		const bool cannot_refract = refraction_ratio * sin_theta > 1.0;

		const auto direction = cannot_refract || reflectance(cos_theta, refraction_ratio) > utils::random_double()
			                 ? unit_direction.reflected(hr.normal)
			                 : unit_direction.refracted(hr.normal, refraction_ratio);

		return ScatterResult{
			Color{1.0, 1.0, 1.0},
			Ray{hr.position, direction}
		};
	}
};

static_assert(Material<Dielectric>);

template <Material... Ms>
class MaterialList {
	static_assert(utils::are_distinct<Ms..>_v, "Some type appears more then one time");

	std::tuple<std::vector<Ms>...> materials;
	
	//probably better option:
	//std::vector<std::variant<Ms...>> materials;

public:
	template <Material M, typename... Ts>
	[[nodiscard]]
	MaterialIndex emplace_material(Ts... args) {
		static_assert(utils::is_in_pack_v<M, Ms...>, "This material is not in the list");
		static_assert(std::is_constructible_v<M, Ts..>, "Cannot construct material from given arguments");

		auto& vec = std::get<std::vector<M>>(materials);
		vec.emplace_back(std::forward<Ts>(args)...);
		return {
			utils::first_occurance<M, Ms...>::value,
			vec.size() - 1
		};
	}

	[[nodiscard]]
	std::optional<ScatterResult> get_scatter_result(const Ray& ray, const HitRecord& hit) const {
		auto visitor = [&]<Material T>(const std::vector<T>& v) {
			return v[hit.material.vector_index].scatter(ray, hit);
		};

		return utils::visit_tuple(materials, visitor, hit.material.type_index);
	}
};
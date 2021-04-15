#pragma once

#include "Hitable.h"
#include "Ray.h"
#include "HitRecord.h"
#include "utils.h"

#include <vector>
#include <tuple>
#include <optional>

template <Hittable... Hs>
class Scene {
	static_assert(utils::are_distinct_v<Hs...>, "Some type appears more then one time");

	std::tuple<std::vector<Hs>... > objects;

	template <typename T>
	auto& get_vector() {
		static_assert(utils::is_in_pack_v<T, Hs...>);
		return std::get<std::vector<T>>(objects);
	}

	template <typename T>
	const auto& get_vector() const {
		static_assert(utils::is_in_pack_v<T, Hs...>);
		return std::get<std::vector<T>>(objects);
	}

public:
	void clear() {
		(std::get<std::vector<Hs>>(objects).clear(), ...);
	}

	template <Hittable T>
	auto push_back(T&& object) {
		static_assert(utils::is_in_pack_v<T, Hs...>, "This type is not in the list");
		return get_vector<T>().push_back(std::forward<T>(object));
	}

	template <Hittable T, typename... Ts>
	void emplace_back(Ts&&... args) {
		static_assert(utils::is_in_pack<T, Hs...>::value, "Given type is not in the list");
		static_assert(std::is_constructible_v<T, Ts...>, "Cannot construct hittable from given arguments");

		get_vector<T>().emplace_back(std::forward<Ts>(args)...);
	}

	[[nodiscard]]
	std::optional<HitRecord> intersect(const Ray& ray, double t_min, double t_max) const {
		std::optional<HitRecord> ret_value{};
		auto closest_so_far = t_max;

		auto intersect_one = [&]<Hittable T>(const std::vector<T>&v) {
			for (const Hittable auto& object : v) {
				if (auto hit = object.intersect(ray, t_min, closest_so_far); hit) {
					closest_so_far = hit->t;
					ret_value = hit;
				}
			}
		};

		(intersect_one(get_vector<Hs>()), ...);

		return ret_value;
	}
};
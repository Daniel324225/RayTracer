#pragma once

#include "utils.h"

#include <cmath>
#include <ostream>

struct Vec3 {
	double data[3]{};

	constexpr Vec3() = default;
	constexpr Vec3(double x, double y, double z) : data{ x, y, z } {}

	[[nodiscard]] constexpr
	double& x() noexcept {
		return data[0];
	}

	[[nodiscard]] constexpr
	double x() const noexcept {
		return data[0];
	}

	[[nodiscard]] constexpr
	double& y() noexcept {
		return data[1];
	}

	[[nodiscard]] constexpr
	double y() const noexcept {
		return data[1];
	}

	[[nodiscard]] constexpr
	double& z() noexcept {
		return data[2];
	}

	[[nodiscard]] constexpr
	double z() const noexcept {
		return data[0];
	}

	constexpr
	Vec3& operator+=(const Vec3& rhs) noexcept {
		for (auto i : { 0, 1, 2 })
			data[i] += rhs.data[i];
		return *this;
	}

	[[nodiscard]] constexpr
	Vec3 operator-() const noexcept {
		return {
			-data[0],
			-data[1],
			-data[2]
		};
	}

	constexpr
	Vec3& operator-=(const Vec3& rhs) noexcept {
		return *this += -rhs;
	}

	constexpr
	Vec3& operator*=(double rhs) noexcept {
		for (auto i : { 0, 1, 2 })
			data[i] *= rhs;
		return *this;
	}

	constexpr
	Vec3& operator/=(const double rhs) noexcept {
		return *this *= (1 / rhs);
	}

	[[nodiscard]] constexpr
	Vec3 operator+(const Vec3& rhs) const noexcept {
		auto tmp = *this;
		return tmp += rhs;
	}

	[[nodiscard]] constexpr
	Vec3 operator-(const Vec3& rhs) const noexcept {
		return *this + (-rhs);
	}

	[[nodiscard]] constexpr
	Vec3 operator*(double rhs) const noexcept {
		auto tmp = *this;
		return tmp *= rhs;
	}

	[[nodiscard]] constexpr
	friend Vec3 operator*(double lhs, const Vec3& rhs) noexcept {
		return rhs * lhs;
	}

	[[nodiscard]] constexpr
	Vec3 operator/(const double rhs) noexcept {
		auto tmp = *this;
		return tmp /= rhs;
	}

	[[nodiscard]] constexpr
	double dot(const Vec3& rhs) const noexcept {
		return data[0] * rhs.data[0]
			 + data[1] * rhs.data[1]
			 + data[2] * rhs.data[2];
	} 

	[[nodiscard]] constexpr
	Vec3 cross(const Vec3& rhs) const noexcept {
		return { data[1] * rhs.data[2] - data[2] * rhs.data[1],
				 data[2] * rhs.data[0] - data[0] * rhs.data[2],
				 data[0] * rhs.data[1] - data[1] * rhs.data[0] };
	}

	[[nodiscard]] constexpr
	double length_squared() const noexcept {
		return dot(*this);
	}
	[[nodiscard]]
	double length() const noexcept {
		return std::sqrt(length_squared());
	}

	void normalize() noexcept {
		*this /= length();
	}

	[[nodiscard]]
	Vec3 unit() const noexcept {
		auto tmp = *this;
		tmp.normalize();
		return tmp;
	}

	[[nodiscard]] constexpr
	Vec3 reflected(const Vec3& normal) const noexcept {
		return *this - 2 * this->dot(normal) * normal;
	}

	[[nodiscard]] constexpr
	Vec3 refracted(const Vec3& normal, double etai_over_etat) const noexcept {
		auto cos_theta = std::min((-*this).dot(normal), 1.0);
		Vec3 r_out_prep = etai_over_etat * (*this + cos_theta * normal);
		Vec3 r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_prep.length_squared())) * normal;

		return r_out_prep + r_out_parallel;
	}

	[[nodiscard]] constexpr
	Vec3 elementwise_mul(const Vec3& v) const noexcept {
		return {
			data[0] * v.data[0],
			data[1] * v.data[1],
			data[2] * v.data[2]
		};
	}

	[[nodiscard]]
	static Vec3 random() noexcept {
		return { utils::random_double(), utils::random_double(), utils::random_double() };
	}

	[[nodiscard]]
	static Vec3 random(double min, double max) noexcept {
		return { utils::random_double(min, max), utils::random_double(min, max), utils::random_double(min, max) };
	}

	[[nodiscard]]
	static Vec3 random_in_sphere() noexcept {
		while (true) {
			auto v = random(-1, 1);
			if (v.length_squared() < 1) return v;
		}
	}

	[[nodiscard]]
	static Vec3 random_in_sphere(double r) noexcept {
		if (r <= 0) return { 0, 0, 0 };
		const auto r2 = r * r;
		while (true) {
			auto v = random(-r, r);
			if (v.length_squared() < r) return v;
		}
	}

	[[nodiscard]]
	static Vec3 random_unit() noexcept {
		return random_in_sphere().unit();
	}

	[[nodiscard]]
	static Vec3 random_in_unit_disk() noexcept {
		while (true) {
			auto v = Vec3{ utils::random_double(-1, 1), utils::random_double(-1, 1), 0 };
			if (v.length_squared() < 1) return v;
		}
	}

	friend
	std::ostream& operator<<(std::ostream& out, const Vec3& v) {
		return out << v.data[0] << ' ' << v.data[1] << ' ' << v.data[2];
	}

	friend
	Vec3 lerp(const Vec3& a, const Vec3& b, double t)
	{
		return a * (1 - t) + b * t;
	}
};

using Color = Vec3;
using Position = Vec3;
using Direction = Vec3;
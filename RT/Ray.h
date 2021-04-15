#pragma once

#include "Vec3.h"

class Ray {
	Position origin_;
	Direction direction_;

public:
	constexpr Ray(const Position& origin, const Direction& direction) : origin_{ origin }, direction_{ direction } {}

	[[nodiscard]] constexpr
	const Position& origin() const noexcept { return origin_; }

	[[nodiscard]] constexpr
	const Direction& direction() const noexcept { return direction_; }

	constexpr
	Position at(double t) const noexcept {
		return origin_ + t * direction_;
	}
};
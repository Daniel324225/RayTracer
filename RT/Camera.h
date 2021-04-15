#pragma once

#include "Frame.h"
#include "Ray.h"
#include "Sphere.h"
#include "Scene.h"

#include <numbers>
#include <cmath>

class Camera {
	Position origin_;
	Position lower_left_;
	Direction horizontal_;
	Direction vertical_;
	Direction u_, v_, w_;
	double lens_radius_;
	
	[[nodiscard]] constexpr
	static double degrees_to_radians(double degrees) noexcept {
		return degrees / 180.0 * std::numbers::pi_v<double>;
	}

public:
	Camera(
		Vec3 lookfrom,
		Vec3 lookat,
		Vec3 vup,
		double vfov,
		double aspect_ratio,
		double aperture,
		double focus_dist
	) {
		const auto theta = degrees_to_radians(vfov);
		const auto h = std::tan(theta / 2);
		const auto viewport_height = 2.0 * h;
		const auto viewport_width = aspect_ratio * viewport_height;
		
		w_ = (lookfrom - lookat).unit();
		u_ = vup.cross(w_).unit();
		v_ = w_.cross(u_);

		origin_ = lookfrom;
		horizontal_ = focus_dist * viewport_width * u_;
		vertical_ = focus_dist * viewport_height * v_;

		lower_left_ = origin_ - horizontal_ / 2 - vertical_ / 2 - focus_dist * w_;
		lens_radius_ = aperture / 2;
	}

	Ray get_ray(double h, double v) const {
		Vec3 rd = lens_radius_ * Vec3::random_in_unit_disk();
		Vec3 offset = u_ * rd.x() + v_ * rd.y();
		return Ray{
			origin_ + offset,
			lower_left_ + h * horizontal_ + v * vertical_ - origin_ - offset
		};
	}
};


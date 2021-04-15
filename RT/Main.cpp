#include "Vec3.h"
#include "Frame.h"
#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"
#include "utils.h"
#include "Materials.h"

#include <iostream>
#include <limits>

template <typename... Ts>
struct TypeList {};

template <typename Hittables, typename Materials>
class RayTracer {
	static_assert(!std::is_void_v<std::void_t<Hittables>>, "Wrong template arguments");
};

template <Hittable... Hs, Material... Ms>
class RayTracer<TypeList<Hs...>, TypeList<Ms...>> {
	Color ray_color(const Ray& ray, int depth) const {
		if (depth <= 0) return { 0.0, 0.0, 0.0 };

		auto background_color = [](const Ray& ray) -> Color {
			auto t = (ray.direction().unit().y() + 1.0) * 0.5;
			return { lerp(Vec3{1.0, 1.0, 1.0}, Vec3{0.5, 0.7, 1.0}, t) };
		};

		auto hit = scene.intersect(ray, 0.001, std::numeric_limits<double>::infinity());

		if (hit) {
			auto res = materials.get_scatter_result(ray, *hit);
			if (res) {
				return res->attenuation.elementwise_mul(ray_color(res->scattered, depth - 1));
			}
			return Color{ 0, 0, 0 };
		}
		else return background_color(ray);
	}

public:
	Scene<Hs...> scene;
	MaterialList<Ms...> materials;

	Frame render(const Camera& camera, const uint64_t height, const uint64_t width) const {
		const int samples_per_pixel = 1000;
		const int max_depth = 100;

		Frame frame{ height, width };

		for (uint64_t y = 0; y < height; ++y) {
			for (uint64_t x = 0; x < width; ++x) {
				Color color{};
				for (int i = 0; i < samples_per_pixel; ++i) {
					auto h = (x + utils::random_double()) / (width - 1);
					auto v = (height - y + utils::random_double()) / (height - 1);

					const auto r = camera.get_ray(h, v);

					color += ray_color(r, max_depth);
				}
				frame.push_pixel(color / samples_per_pixel);
			}
			std::cout << y << "\n";
		}

		return frame;
	}
};

void default_render() {
	RayTracer<TypeList<Sphere>, TypeList<Lambertian, Metal, Dielectric>> RT{};

	auto ground_material = RT.materials.emplace_material<Lambertian>(Color{ 0.5, 0.5, 0.5 });
	RT.scene.emplace_back<Sphere>(Position{ 0, -1000, 0 }, 1000, ground_material);

	std::discrete_distribution<int> material_dst{ {80, 15, 5} };
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			auto choose_material = material_dst(utils::rng);
			Position center{ a + 0.7 * utils::random_double(), 0.2, b + 0.7 * utils::random_double() };

			if ((center - Position{ 4, 0.2, 0 }).length() > 0.9) {
				MaterialIndex sphere_material = [&] {
					switch (choose_material)
					{
					case 0: {
						const auto color = Color::random().elementwise_mul(Color::random());
						return RT.materials.emplace_material<Lambertian>(color);
					}
					case 1: {
						const auto color = Color::random(0.5, 1);
						const auto fuzz = utils::random_double(0, 0.5);
						return RT.materials.emplace_material<Metal>(color, fuzz);
					}
					default: {
						return RT.materials.emplace_material<Dielectric>(1.5);
					}
					}
				}();

				RT.scene.emplace_back<Sphere>(center, 0.2, sphere_material);
			}
		}
	}

	auto material = RT.materials.emplace_material<Dielectric>(1.5);
	RT.scene.emplace_back<Sphere>(Position{ 0, 1, 0 }, 1.0, material);

	material = RT.materials.emplace_material<Lambertian>(Color{ 0.4, 0.2, 0.1 });
	RT.scene.emplace_back<Sphere>(Position{ -4, 1, 0 }, 1.0, material);

	material = RT.materials.emplace_material<Metal>(Color{ 0.7, 0.6, 0.5 }, 0.0);
	RT.scene.emplace_back<Sphere>(Position{ 4, 1, 0 }, 1, material);

	const uint64_t height = 1200;
	const uint64_t width = height * 3 / 2;
	const auto aspect_ratio = double(width) / height;

	Position lookfrom{ 13, 2, 3 };
	Position lookat{ 0, 0, 0 };
	Direction vup{ 0, 1, 0 };
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	Camera camera {
		lookfrom,
		lookat,
		vup,
		20,
		aspect_ratio,
		aperture,
		dist_to_focus
	};

	RT.render(camera, height, width).to_ppm("out.ppm");
}

int main() {
	default_render();
}
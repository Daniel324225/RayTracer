#pragma once

#include "Vec3.h"
#include <vector>

class Frame {
	uint64_t height;
	uint64_t width;
	std::vector<Color> data;

public:
	Frame(uint64_t height, uint64_t width) : height{ height }, width{ width } {}

	void push_pixel(const Color& c) {
		data.push_back(c);
	}

	bool to_ppm(const char* filename) const;
};
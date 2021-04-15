#include "Frame.h"
#include "utils.h"

#include <fstream>
#include <string>

bool Frame::to_ppm(const char* filename) const {
	if (height * width != data.size()) return false;
	auto file = std::ofstream{ filename, std::ios::binary };
	if (!file) return false;

	file.write("P6 ", 3);
	auto tmp = std::to_string(width) + " " + std::to_string(height) + " 255 ";
	file.write(tmp.data(), tmp.size());

	auto write_color = [&file](const Color& c) {
		unsigned char data[3];
		for (auto i : { 0, 1, 2 })
			data[i] = static_cast<unsigned char>(utils::clamp(256 * std::sqrt(c.data[i]), 0.0, 255.0));
			//data[i] = static_cast<int>(256 * clamp(std::sqrt(c.data[i]), 0.0, 0.999));

		file.write((char*)data, 3);
	};

	for (const auto& c : data) {
		write_color(c);
	}

	return file.good();
}
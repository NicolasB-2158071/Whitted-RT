#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>

class Image {
public:
	Image(unsigned short width, unsigned short height);

	unsigned GetWidth() const;
	unsigned GetHeight() const;

	glm::vec3 GetPixel(unsigned short i, unsigned short j) const;
	void SetPixel(unsigned short i, unsigned short j, const glm::vec3& rgb);

	void Save(const std::string& path) const;

private:
	unsigned short m_width;
	unsigned short m_height;
	std::vector<float> m_pixels;
};
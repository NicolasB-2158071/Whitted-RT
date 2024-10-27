#include <fstream>
#include <iostream>
#include <algorithm>
#include "Image.h"
#include "toojpeg.h"

Image::Image(unsigned short width, unsigned short height) : m_width{ width }, m_height{ height }, m_pixels(m_width * m_height * 3, 0.0) {}

unsigned Image::GetWidth() const {
    return m_width;
}

unsigned Image::GetHeight() const {
    return m_height;
}

glm::vec3 Image::GetPixel(unsigned short i, unsigned short j) const {
    int idx{ 3 * j + 3 * m_width * i };
    return glm::vec3{ m_pixels[idx], m_pixels[idx + 1], m_pixels[idx + 2] };
}

void Image::SetPixel(unsigned short i, unsigned short j, const glm::vec3& rgb) {
    int idx{ 3 * j + 3 * m_width * i };
    for (int k = 0; k < 3; ++k) {
        m_pixels[idx + k] = rgb[k];
    }
}

void Image::Save(const std::string& path) const {
    std::ofstream file{ path, std::ios::binary };
    std::vector<unsigned char> framebuffer(m_width * m_height * 3);
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            glm::vec3  rgb = GetPixel(i, j);
            for (int k = 0; k < 3; ++k) {
                framebuffer[3 * j + 3 * m_width * i + k] = 
                    std::clamp(static_cast<unsigned char>(255.0f * rgb[k]), static_cast<unsigned char>(0), static_cast<unsigned char>(255));
            }
        }
    }
    TooJpeg::writeJpeg([](unsigned char byte, void* data) -> void {
        *reinterpret_cast<std::ofstream*>(data) << byte;
    }, &file, framebuffer.data(), m_width, m_height);
    file.close();
}

#pragma once

#include <glm/vec3.hpp>

class Camera {
public:
	Camera(size_t imageWidth, size_t imageHeight, const glm::vec3& center = glm::vec3{0.0f, 0.0f, 0.0f}, float focalLength = 1.0f);

	glm::vec3 GetViewportPixelCenter(int u, int v) const;
	glm::vec3 GetCameraCenter() const;

private:
	void CalculateViewportLeftPixelCenter(const glm::vec3& viewportU, const glm::vec3& viewportV);

	float m_focalLength;
	glm::vec3 m_center;
	glm::vec3 m_pixelDeltaU;
	glm::vec3 m_pixelDeltaV;
	glm::vec3 m_viewportLeftPixelCenter;
};

// Simple camera: https://raytracing.github.io/books/RayTracingInOneWeekend.html#rays,asimplecamera,andbackground
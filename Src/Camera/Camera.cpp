#include <glm/vec2.hpp>
#include "Camera.h"
#include "../Utility/Random.h"

Camera::Camera(
	size_t imageWidth,
	size_t imageHeight,
	const glm::vec3& center,
	float focalLength
) : m_focalLength{ focalLength }, m_center{ center } {
	float viewportHeight = 2.0f;
	float viewportWidth = viewportHeight * (static_cast<float>(imageWidth / imageHeight));

	glm::vec3 viewportU{ viewportWidth, 0, 0 };
	glm::vec3 viewportV{ 0, -viewportHeight, 0 };
	m_pixelDeltaU = viewportU / static_cast<float>(imageWidth);
	m_pixelDeltaV = viewportV / static_cast<float>(imageHeight);
	CalculateViewportLeftPixelCenter(viewportU, viewportV);
}

glm::vec3 Camera::GetViewportPixelCenter(int u, int v) const {
	glm::vec2 offset{ RandomDouble() - 0.5, RandomDouble() - 0.5 };
	return m_viewportLeftPixelCenter + ((static_cast<float>(u) + offset.x) * m_pixelDeltaU) + ((static_cast<float>(v) + offset.y) * m_pixelDeltaV);
}

glm::vec3 Camera::GetCameraCenter() const {
	return m_center;
}

void Camera::CalculateViewportLeftPixelCenter(const glm::vec3& viewportU, const glm::vec3& viewportV) {
	glm::vec3 viewportLeft{
		m_center - glm::vec3{0, 0, m_focalLength} -
		(viewportU / 2.0f) -
		(viewportV / 2.0f)
	};
	m_viewportLeftPixelCenter = viewportLeft + static_cast<float>(0.5) * (m_pixelDeltaU + m_pixelDeltaV);
}

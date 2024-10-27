#pragma once

#include <embree4/rtcore.h>
#include "tiny_obj_loader.h"
#include <string>
#include "Shape.h"
#include "Light.h"

class Scene {
public:
	Scene(float shadowIntensity = 0.3f);
	Scene(const std::string& inputFile, float shadowIntensity = 0.3f);

	void LoadObj(const std::string& inputFile);
	RTCScene Build(RTCDevice device) const;

	size_t GetNumShapes() const;
	const Shape& GetShape(int id) const;
	size_t GetNumPointLights() const;
	const Light& GetPointLight(int id) const;

	glm::vec3 GetCameraCoordinates() const;
	float GetShadowIntensity() const;

private:
	std::string m_name;
	std::vector<Shape> m_shapes;
	std::vector<Light> m_pointLights;
	glm::vec3 m_cameraCoordinates;
	float m_shadowIntensity;

	tinyobj::ObjReader CreateReader(const std::string& inputFile) const;
	Shape ReadShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const tinyobj::material_t& mat) const;
	Light ReadLight(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const tinyobj::material_t& mat) const;
	const glm::vec3 ReadCamera(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att) const;
};
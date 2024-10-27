#pragma once

#include "tiny_obj_loader.h"
#include <glm/vec3.hpp>

enum class ILLUM {
	HIGHLIGHT_ON = 2,
	REFLECTION_ON_FRESNEL = 5,
	REFLECTION_REFRACTION_ON_FRESNEL = 7
};

struct Material {
	glm::vec3 ka;
	glm::vec3 kd;
	float ior;
	ILLUM type;

	void createFrom(const tinyobj::material_t& mat) {
		ka = glm::vec3{ mat.ambient[0], mat.ambient[1] , mat.ambient[2] };
		kd = glm::vec3{ mat.diffuse[0], mat.diffuse[1] , mat.diffuse[2] };
		ior = mat.ior;
		type = ILLUM{ mat.illum };
	}
};
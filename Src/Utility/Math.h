#pragma once

#include <glm/glm.hpp>

// https://github.com/scratchapixel/scratchapixel-code/blob/main/ray-tracing-overview/whitted.cpp#L329
float FresnelKr(const glm::vec3& I, const glm::vec3& N, float ior) {
    float cosi = glm::clamp(glm::dot(I, N), -1.0f, 1.0f);
    float etai = 1.0f, etat = ior;
    if (cosi > 0.0f) {
        std::swap(etai, etat);
    }
    float sint = etai / etat * glm::sqrt(glm::max(0.f, 1 - cosi * cosi));
    if (sint >= 1) {
        return 1.0f;
    } else {
        float cost = glm::sqrt(glm::max(0.f, 1 - sint * sint));
        cosi = glm::abs(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

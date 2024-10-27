#pragma once

#include <embree4/rtcore.h>
#include "Scene/Scene.h"
#include "Image/Image.h"

class Raytracer {
public:
	Raytracer(int repetitions = 1);
	~Raytracer();
	
	void Trace(const Scene& scene);

private:
	RTCDevice m_device;
	void ErrorFunction(void* userPtr, enum RTCError error, const char* str) const;
	int m_repititions; // Spatial anti-aliasing, super sampling

	const Scene* m_scene;
	Image m_image;
	RTCRayHit CreateRayHit(
		const glm::vec3& origin,
		const glm::vec3& dir,
		float tnear = 0.0f,
		float tfar = std::numeric_limits<float>::infinity(),
		int mask = -1,
		unsigned int geomID = RTC_INVALID_GEOMETRY_ID,
		unsigned int primID = RTC_INVALID_GEOMETRY_ID
	) const;
	glm::vec3 CastRay(RTCScene rtcScene, RTCRayHit& rayHit, int depth);
	bool InShadow(RTCScene rtcScene, const glm::vec3& pos, const Light& light) const;
};
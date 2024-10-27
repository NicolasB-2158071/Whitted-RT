#include "Raytracer.h"
#include "spdlog/spdlog.h"
#include <glm/glm.hpp>
#include "Camera/Camera.h"
#include "Utility/Math.h"

Raytracer::Raytracer(int repetitions) : m_device{ rtcNewDevice(nullptr) }, m_scene{ nullptr }, m_image{ 800, 800 }, m_repititions{ repetitions } {
	if (!m_device) {
		spdlog::error("Embree: cannot create device {}", static_cast<int>(rtcGetDeviceError(nullptr)));
	}
	rtcSetDeviceErrorFunction(m_device, nullptr, nullptr);
}

Raytracer::~Raytracer() {
	rtcReleaseDevice(m_device);
}

void Raytracer::ErrorFunction(void* userPtr, enum RTCError error, const char* str) const {
	spdlog::error("Embree: {} {}", static_cast<int>(error), str);
}

RTCRayHit Raytracer::CreateRayHit(
	const glm::vec3& origin,
	const glm::vec3& dir,
	float tnear,
	float tfar,
	int mask,
	unsigned int geomID,
	unsigned int primID
) const {
	RTCRayHit rayHit;
	rayHit.ray.org_x = origin.x; rayHit.ray.org_y = origin.y; rayHit.ray.org_z = origin.z;
	rayHit.ray.dir_x = dir.x; rayHit.ray.dir_y = dir.y; rayHit.ray.dir_z = dir.z;
	rayHit.ray.tnear = tnear;
	rayHit.ray.tfar = tfar;
	rayHit.ray.mask = mask;
	rayHit.hit.geomID = geomID;
	rayHit.hit.primID = primID;
	return rayHit;
}

glm::vec3 Raytracer::CastRay(RTCScene rtcScene, RTCRayHit& rayHit, int depth) {
	if (depth == 0) {
		return glm::vec3{ 0.0f };
	}
	
	glm::vec3 color{ 0.0f };
	rtcIntersect1(rtcScene, &rayHit);
	if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
		glm::vec3 dir{
			rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z
		};
		glm::vec3 pos{
			glm::vec3{rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z} +
			dir * rayHit.ray.tfar
		};
		const Shape& shape{ m_scene->GetShape(rayHit.hit.geomID) };
		glm::vec3 normal{};
		rtcInterpolate1(
			rtcGetGeometry(rtcScene, rayHit.hit.geomID),
			rayHit.hit.primID,
			rayHit.hit.u,
			rayHit.hit.v,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			0,
			&normal.x,
			nullptr,
			nullptr,
			3
		);
		normal = glm::normalize(normal);

		for (int i = 0; i < m_scene->GetNumPointLights(); ++i) {
			const Light& light{ m_scene->GetPointLight(i) };
			glm::vec3 amb{ shape.material.ka * light.material.ka };
			glm::vec3 diff{ shape.material.kd * light.material.kd * glm::max(0.0f, glm::dot(glm::normalize(light.pos - pos), normal)) };
			glm::vec3 c{ (amb + diff) / glm::distance(light.pos, pos) };
			//if (InShadow(rtcScene, pos, light)) {
			//	c *= m_scene->GetShadowIntensity();
			//}
			color += c;
		}

		bool inAir{ glm::dot(dir, normal) < 0.0f };
		glm::vec3 refl{ glm::normalize(glm::reflect(dir, normal)) };
		glm::vec3 refr{ glm::normalize(glm::refract(dir, inAir ? normal : -normal, inAir ? 1.0f / shape.material.ior : shape.material.ior)) }; // Current medium / ingoing medium
		RTCRayHit reflRay{ CreateRayHit(pos, refl, 0.001f) };
		RTCRayHit refrRay{ CreateRayHit(pos, refr, 0.001f) };
		float kr{ FresnelKr(dir, normal, shape.material.ior) };
		switch (shape.material.type) {
			case ILLUM::REFLECTION_ON_FRESNEL: {
				color += CastRay(rtcScene, reflRay, depth - 1) * kr;
				break;
			}
			case ILLUM::REFLECTION_REFRACTION_ON_FRESNEL: {
				color += CastRay(rtcScene, reflRay, depth - 1) * kr;
				color += CastRay(rtcScene, refrRay, depth - 1) * (1 - kr);
				break;
			}
			default: {
				break;
			}
		}
	}
	return glm::clamp(color, 0.0f, 1.0f);
}

bool Raytracer::InShadow(RTCScene rtcScene, const glm::vec3& pos, const Light& light) const {
		glm::vec3 lp{ light.pos };
		RTCRayHit shadowRay{ CreateRayHit(pos, glm::normalize(lp - pos), 0.001f, glm::distance(lp, pos) - 0.001f) };
		// Default, faster for shadow rays 
		RTCOccludedArguments sargs;
		rtcInitOccludedArguments(&sargs);
		rtcOccluded1(rtcScene, &shadowRay.ray, &sargs);
		// When no intersection is found, the ray data is not updated. In case a hit was found, the tfar component of the ray is set to -inf.
		return shadowRay.ray.tfar < 0.0f;
}

void Raytracer::Trace(const Scene& scene) {
	// OpenMP for parallel
	m_scene = &scene;
	RTCScene rtcScene{ m_scene->Build(m_device) };
	Camera camera{ m_image.GetWidth(), m_image.GetHeight(), scene.GetCameraCoordinates() };
	for (unsigned short j = 0; j < m_image.GetHeight(); ++j) {
		for (unsigned short i = 0; i < m_image.GetWidth(); ++i) {
			glm::vec3 col{ 0.0f };
			for (int k = 0; k < m_repititions; ++k) {
				glm::vec3 pixelCenter{ camera.GetViewportPixelCenter(i, j) };
				glm::vec3 cameraCenter{ camera.GetCameraCenter() };
				glm::vec3 rayDirection{ glm::normalize(pixelCenter - cameraCenter) };


				RTCRayHit rayHit{ CreateRayHit(cameraCenter, rayDirection) };
				col += CastRay(rtcScene, rayHit, 5);
			}
			m_image.SetPixel(j, i, glm::clamp(col / static_cast<float>(m_repititions), 0.0f, 1.0f));
		}
	}
	rtcReleaseScene(rtcScene);
	m_image.Save("./CB_7s_refractionReflectionViaFresnel.jpeg");
}
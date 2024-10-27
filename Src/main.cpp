#include "spdlog/spdlog.h"
#include "Raytracer.h"

int main() {
	// als parameter aanpassen
	spdlog::set_level(spdlog::level::trace);

	Scene scene{ "../Res/CornellBox-Sphere.obj" };
	Raytracer rt{5};
	rt.Trace(scene);

	return 0;
}

// TODO:
// Camera
	// Complexer camera
	// Interactiveness?
// Scene
	// Add texture support
	// Linear interpolate texcoords

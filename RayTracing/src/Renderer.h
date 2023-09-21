#pragma once

#include "Walnut/Image.h"
#include "Walnut/Random.h"

#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#define RENDERMODE_CHERNO

class Renderer {
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera, Scene& scene);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	struct HitPayload {
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};
	glm::vec4 PerPixel(uint32_t x, uint32_t y); //similar RayGen in vulkan/directX

	HitPayload TraceRay(Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

#ifndef RENDERMODE_CHERNO
	// create camera
	Camera c;
	SpehricalCamera sc;
	// create sphere
	Sphere s;
#endif
};
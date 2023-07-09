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
	glm::vec4 Renderer::TraceRay(Ray& ray, Scene& scene);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

#ifndef RENDERMODE_CHERNO
	// create camera
	Camera c;
	SpehricalCamera sc;
	// create sphere
	Sphere s;
#endif
};
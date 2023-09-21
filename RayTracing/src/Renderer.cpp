#include "Renderer.h"
#include <iostream>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = a << 24 | b << 16 | g << 8 | r;
		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height) {
	if (m_FinalImage) {
		if (width == m_FinalImage->GetWidth() && height == m_FinalImage->GetHeight()) {
			return;
		}

		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
		
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

#ifndef RENDERMODE_CHERNO
	// create camera
	c = Camera(Vec3(0, 0, 2), Vec3(2.0 * width / height, 0, 0), Vec3(0, 0, -2), 0.5, width, height);
	float widthFieldOfView = 50;
	sc = SpehricalCamera(Vec3(0, 0, 0), 0.5, 3.14 / 180 * 90, 3.14 / 180 * widthFieldOfView, 3.14 / 180 * 90, 3.14 / 180 * widthFieldOfView * height / width);
	// create sphere
	s = Sphere(0, 100, 0, 5);
#endif
}

void Renderer::Render(const Camera& camera, Scene& scene) {
	// render every pixel
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			//glm::vec2 coord = { (float) x / m_FinalImage->GetWidth(), (float) y / m_FinalImage->GetHeight() };
			//coord = coord * 2.0f - 1.0f;
			//glm::vec4 color = PerPixel(coord);
			glm::vec4 color = PerPixel(x, y);  
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(color); //Walnut::Random::UInt() | 0xff000000;
		}
	}
	
	m_FinalImage->SetData(m_ImageData);
}

Renderer::HitPayload Renderer::TraceRay(Ray& ray) {
#ifndef RENDERMODE_CHERNO
	//if (s.IntersectsRay(c.getRay(j, i))) {
	if (s.IntersectsRay(sc.getRay(coord))) {
		return glm::vec4(1, 0, 1, 1);
	}
	else {
		return glm::vec4(0, 0, 0, 1);
	}
#else
	int closestObjectIndex = -1;
	float closestObjectDistance = FLT_MAX;
	for (int i = 0; i < m_ActiveScene->objects.size(); i++) {
		float objectDistance = m_ActiveScene->objects[i]->IntersectsRay(ray);
		if (objectDistance > 0 && objectDistance < closestObjectDistance) {
			closestObjectIndex = i;
			closestObjectDistance = objectDistance;
			//std::cout << "here" << std::endl;
		}
	}

	//std::cout << closestObjectIndex << ", " << closestObjectDistance << std::endl;

	if (closestObjectIndex == -1) {
		return Miss(ray);
	}

	return ClosestHit(ray, closestObjectDistance, closestObjectIndex);
#endif
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) { //similar RayGen in vulkan/directX
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_FinalImage->GetWidth() + x];

	glm::vec3 lightDir = m_ActiveScene->lightDir;
	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++) {
		Renderer::HitPayload payload = TraceRay(ray);

		if (payload.HitDistance < 0) {
			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color += multiplier * skyColor;
			multiplier *= 0.7f;
			break;
		}

		float lightIntensity = glm::max(0.0f, glm::dot(payload.WorldNormal, -lightDir));
		SceneObject* object = m_ActiveScene->objects[payload.ObjectIndex];
		glm::vec3 sphereColor = object->getAlbedo() * lightIntensity;
		color += multiplier * sphereColor;
		multiplier *= 0.7f;

		// bounce ray
		// move origin a little bit to not intersect again with object we're bouncing off of
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; 
		//glm::vec3 oldDirectionProj = glm::dot(ray.Direction, payload.WorldNormal) * payload.WorldNormal;
		//ray.Direction = payload.WorldNormal * 2.0f + oldDirectionProj;
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex) {
	SceneObject* closestObject = m_ActiveScene->objects[objectIndex];
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	payload.WorldPosition = ray.Origin + hitDistance * ray.Direction;
	payload.WorldNormal = closestObject->getNormal(payload.WorldPosition);

	return payload;
}
Renderer::HitPayload Renderer::Miss(const Ray& ray) {
	HitPayload payload;
	payload.HitDistance = -1.0f;

	return payload;
}

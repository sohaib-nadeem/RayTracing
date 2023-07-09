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
	const glm::vec3& rayOrigin = camera.GetPosition();
	const std::vector<glm::vec3>& rayDirections = camera.GetRayDirections();
	Ray ray(camera.GetPosition(), glm::vec3(0));

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			//glm::vec2 coord = { (float) x / m_FinalImage->GetWidth(), (float) y / m_FinalImage->GetHeight() };
			//coord = coord * 2.0f - 1.0f;
			//glm::vec4 color = PerPixel(coord);
			ray.Direction = rayDirections[y * m_FinalImage->GetWidth() + x];
			glm::vec4 color = TraceRay(ray, scene);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(color); //Walnut::Random::UInt() | 0xff000000;
		}
	}
	
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(Ray& ray, Scene& scene) {
#ifndef RENDERMODE_CHERNO
	//if (s.IntersectsRay(c.getRay(j, i))) {
	if (s.IntersectsRay(sc.getRay(coord))) {
		return glm::vec4(1, 0, 1, 1);
	}
	else {
		return glm::vec4(0, 0, 0, 1);
	}
#else
	glm::vec3 lightDir = scene.lightDir;

	SceneObject* closestObject = nullptr;
	float closestObjectDistance = FLT_MAX;
	for (int i = 0; i < scene.objects.size(); i++) {
		float objectDistance = scene.objects[i]->IntersectsRay(ray);
		if (objectDistance > 0 && objectDistance < closestObjectDistance) {
			closestObject = scene.objects[i];
			closestObjectDistance = objectDistance;
			//std::cout << "here" << std::endl;
		}
	}

	//std::cout << closestObjectIndex << ", " << closestObjectDistance << std::endl;

	if (closestObject == nullptr) {
		return glm::vec4(0, 0, 0, 1);
	}
	
	glm::vec3 hitPoint = ray.Origin + closestObjectDistance * ray.Direction;
	glm::vec3 normal = closestObject->getNormal(hitPoint);
	float lightIntensity = glm::max(0.0f, glm::dot(normal, -lightDir));
	return glm::vec4(closestObject->getAlbedo() * lightIntensity, 1.0f);
#endif
}
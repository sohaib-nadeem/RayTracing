#pragma once

#include <glm/glm.hpp>

// remove
//#include "Walnut/Application.h" 
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"

#define RENDERMODE_CHERNO

#ifndef RENDERMODE_CHERNO

#include <cmath>
#include <iostream>

class Quadratic {
public:
	double a, b, c;

public:
	Quadratic(double a, double b, double c) : a{ a }, b{ b }, c{ c } {}

	bool hasRoot() {
		return (b * b - 4 * a * c) >= 0.000001;
	}
};

class Linear {
public:
	double a, b;

public:
	Linear(double a, double b) : a{ a }, b{ b } {}

	Quadratic Square() {
		return Quadratic(a * a, 2 * a * b, b * b);
	}
};

class Vec3 {
public:
	float x, y, z;

public:
	Vec3() {}
	Vec3(float x, float y, float z) : x{ x }, y{ y }, z{ z } {};

	Vec3 Scale(float a) {
		return Vec3(x * a, y * a, z * a);
	}

	Vec3 operator+(Vec3& a) {
		return Vec3(x + a.x, y + a.y, z + a.z);
	}

	Vec3 operator-(Vec3& a) {
		return Vec3(x - a.x, y - a.y, z - a.z);
	}

	Vec3 UnitVector() {
		float length = std::sqrt(x * x + y * y + z * z);
		return Vec3(x / length, y / length, z / length);
	}

	Vec3 Cross(Vec3& a) {
		return Vec3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
	}
};

class Ray {
public:
	Vec3 origin;
	Vec3 direction;

public:
	Ray(Vec3 origin, Vec3 direction) : origin{ origin }, direction{ direction } {};
};

class Camera {
	Vec3 topLeft, widthVector, heightVector;
	Vec3 lensCenter;
	uint32_t screenWidth, screenHeight;

public:
	Camera() {}
	Camera(Vec3 topLeft, Vec3 widthVector, Vec3 heightVector, Vec3 lensCenter, uint32_t screenWidth, uint32_t screenHeight) :
		topLeft{ topLeft }, widthVector{ widthVector }, heightVector{ heightVector }, lensCenter{ lensCenter }, screenWidth{ screenWidth }, screenHeight{ screenHeight } {}
	Camera(Vec3 topLeft, Vec3 widthVector, Vec3 heightVector, float lensDistance, uint32_t screenWidth, uint32_t screenHeight) :
		topLeft{ topLeft }, widthVector{ widthVector }, heightVector{ heightVector }, screenWidth{ screenWidth }, screenHeight{ screenHeight } {
		Vec3 screenCenter = topLeft + widthVector.Scale(0.5) + heightVector.Scale(0.5);
		Vec3 unitNormal = widthVector.Cross(heightVector).UnitVector();
		lensCenter = screenCenter + unitNormal.Scale(lensDistance);
		std::cout << lensCenter.x << " " << lensCenter.y << " " << lensCenter.z << std::endl;
	}
	Ray getRay(int col, int row) { // i for x, j for y
		// origin is pixel
		// direction is lensCenter - pixel 
		// add 0.5 to col/row?
		Vec3 pixelCenter = topLeft + widthVector.Scale((float)col / screenWidth) + heightVector.Scale((float)row / screenHeight);
		return Ray(pixelCenter, lensCenter - pixelCenter);
	}
};

class SpehricalCamera {
	Vec3 lensCenter;
	// axes
	// angle ranges
	float thetaCenter, thetaRange, phiCenter, phiRange;
	float lensDistance; // not necessary

public:
	SpehricalCamera() {}
	SpehricalCamera(Vec3 lensCenter, float lensDistance, float thetaCenter, float thetaRange, float phiCenter, float phiRange) :
		lensCenter{ lensCenter }, lensDistance{ lensDistance }, thetaCenter{ thetaCenter }, thetaRange{ thetaRange }, phiCenter{ phiCenter }, phiRange{ phiRange } {}
	Ray getRay(glm::vec2 coord) { // orientation off
		float theta = thetaCenter + thetaRange / 2 * coord.x;
		float phi = phiCenter + phiRange / 2 * coord.y;
		Vec3 direction = Vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
		return Ray(lensCenter, direction);
	}
};

class Sphere {
public:
	Vec3 center;
	double radius;

public:
	Sphere() {}
	Sphere(Vec3 center, double radius) : center{ center }, radius{ radius } {};
	Sphere(float x, float y, float z, double radius) : center(x, y, z), radius{ radius } {};

	bool IntersectsRay(Ray r) {
		Quadratic xComp = Linear(r.direction.x, r.origin.x - center.x).Square();
		Quadratic yComp = Linear(r.direction.y, r.origin.y - center.y).Square();
		Quadratic zComp = Linear(r.direction.z, r.origin.z - center.z).Square();
		return Quadratic(xComp.a + yComp.a + zComp.a, xComp.b + yComp.b + zComp.b, xComp.c + yComp.c + zComp.c - radius * radius).hasRoot();
	}
};
#endif

class Ray {
public:
	glm::vec3 Origin;
	glm::vec3 Direction;

public:
	Ray(const glm::vec3& Origin, const glm::vec3& Direction) : Origin{ Origin }, Direction{ Direction } {};
};

class SceneObject {
public:
	virtual float IntersectsRay(Ray& ray) = 0;
	virtual glm::vec3 getAlbedo() = 0;
	virtual glm::vec3 getNormal(glm::vec3& hitPoint) = 0;
	virtual void ImGuiParameters() = 0;
};

class Sphere : public SceneObject {
public:
	glm::vec3 center;
	float radius;
	glm::vec3 albedo;

public:
	Sphere(glm::vec3& center, float radius, glm::vec3& albedo) : center{ center }, radius{ radius }, albedo{ albedo } {};

	float IntersectsRay(Ray& ray) override {
		glm::vec3 originMinusCenter = ray.Origin - center;
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(ray.Direction, originMinusCenter);
		float c = glm::dot(originMinusCenter, originMinusCenter) - radius * radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			return -1;

		//float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

		return closestT;
	}

	glm::vec3 getAlbedo() override {
		return albedo;
	}

	glm::vec3 getNormal(glm::vec3& hitPoint) override {
		return glm::normalize(hitPoint - center);
	}

	void ImGuiParameters() override {
		ImGui::DragFloat3("Position", glm::value_ptr(center), 0.1f);
		ImGui::DragFloat("Radius", &radius, 0.1f);
		ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), 0.1f);
	}
};

class Scene {
public:
	std::vector<SceneObject*> objects;
	glm::vec3 lightDir;

public:
	Scene(glm::vec3& lightDir) : lightDir(lightDir) {}
	~Scene() {
		for (int i = 0; i < objects.size(); i++) {
			delete objects[i];
		}
	}
	void addObject(SceneObject* object) { objects.push_back(object); }
};
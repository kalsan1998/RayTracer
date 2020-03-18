// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

class Ray
{
public:
	Ray(const glm::vec3 &A, const glm::vec3 &B);
	glm::vec3 A;
	glm::vec3 B;

	// Precompute the squares
	glm::vec3 A_2;
	glm::vec3 B_A;
	glm::vec3 B_A_2;

	glm::vec3 GetPoint(double t) const;
};

bool ObjectBetween(const SceneNode *node, const Ray &ray, const glm::mat4 &m);

void Render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height
	Image &image,

	// Viewing parameters
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double fovy,

	// Lighting parameters
	const glm::vec3 &ambient,
	const std::list<Light *> &lights);

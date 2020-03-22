// Winter 2020

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle(size_t pv1, size_t pv2, size_t pv3)
		: v1(pv1), v2(pv2), v3(pv3)
	{
	}
};

// A polygonal mesh.
class Mesh : public Primitive
{
public:
	Mesh(const std::string &fname);
	~Mesh();
	bool DoesRayIntersect(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point) const override;

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

	Box *m_bounding_volume;
	glm::mat4 m_transform;
	glm::mat3 m_norm_transform;
};

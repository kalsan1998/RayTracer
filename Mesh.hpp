// Winter 2020

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1, v2, v3;
	size_t n1, n2, n3;
	size_t u1, u2, u3;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uv;
	double area;

	Triangle(size_t pv1, size_t pv2, size_t pv3);
};

// A polygonal mesh.
class Mesh : public Primitive
{
public:
	Mesh(const std::string &fname);
	~Mesh();
	bool RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const override;

private:
	void UpdateTriangle(Triangle &tri);
	void PhongInterpolate(const Triangle &tri, const glm::vec3 &point, glm::vec3 &normal, glm::vec2 &uv) const;

	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	std::vector<glm::vec2> m_uv;
	std::vector<glm::vec3> m_normals;

	Box *m_bounding_volume;
	glm::mat4 m_transform;
	glm::mat3 m_norm_transform;
};

// Winter 2020

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <glm/ext.hpp>

#include "Mesh.hpp"

Triangle::Triangle(size_t pv1, size_t pv2, size_t pv3)
	: v1(pv1), v2(pv2), v3(pv3), vertices(3), normals(3), uv(3)
{
}

double TriangleArea(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 c)
{
	return glm::length(glm::cross(a - b, a - c)) / 2.0;
}

void Mesh::UpdateTriangle(Triangle &tri)
{
	tri.vertices = {m_vertices[tri.v1], m_vertices[tri.v2], m_vertices[tri.v3]};
	tri.area = TriangleArea(tri.vertices[0], tri.vertices[1], tri.vertices[2]);
	if (!m_normals.size() || tri.n1 < 0)
	{
		glm::vec3 v1 = tri.vertices[0];
		glm::vec3 v2 = tri.vertices[1];
		glm::vec3 v3 = tri.vertices[2];
		glm::vec3 A = v2 - v1;
		glm::vec3 B = v3 - v1;
		glm::vec3 norm = glm::normalize(glm::cross(A, B));
		tri.normals = {norm, norm, norm};
	}
	else
	{
		tri.normals = {m_normals[tri.n1], m_normals[tri.n2], m_normals[tri.n3]};
	}
	if (!m_uv.size() || tri.u1 < 0)
	{
		tri.uv = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
	}
	else
	{
		tri.uv = {m_uv[tri.u1], m_uv[tri.u2], m_uv[tri.u3]};
	}
}

std::vector<int> FaceTermToInt(std::string &s)
{
	std::vector<std::string> spl;
	boost::split(spl, s, boost::is_any_of("/"));
	int a = std::stoi(spl[0]);
	if (spl.size() == 1)
	{
		return {a, -1, -1};
	}
	if (spl[1] != "")
	{
		return {a, std::stoi(spl[1]), std::stoi(spl[2])};
	}
	return {a, -1, std::stoi(spl[2])};
}

Mesh::Mesh(const std::string &fname)
	: m_vertices(), m_faces(), m_uv(), m_normals()
{
	std::string line;
	std::string code;
	double vx, vy, vz;
	double u, v;
	double nx, ny, nz;
	std::string s1, s2, s3, s4;

	double min_x = std::numeric_limits<double>::infinity();
	double min_y = min_x;
	double min_z = min_x;
	double max_x = -min_x;
	double max_y = -min_x;
	double max_z = -min_x;

	std::ifstream ifs(fname.c_str());
	if (ifs.fail())
	{
		std::cerr << "Unable to open file: " << fname << std::endl;
	}
	while (std::getline(ifs, line))
	{
		std::istringstream iss(line);
		iss >> code;
		if (code == "v")
		{
			iss >> vx >> vy >> vz;
			max_x = std::max(max_x, vx);
			max_y = std::max(max_y, vy);
			max_z = std::max(max_z, vz);
			min_x = std::min(min_x, vx);
			min_y = std::min(min_y, vy);
			min_z = std::min(min_z, vz);

			m_vertices.push_back(glm::vec3(vx, vy, vz));
		}
		else if (code == "f")
		{
			iss >> s1 >> s2 >> s3;

			std::vector<int> c1 = FaceTermToInt(s1);
			std::vector<int> c2 = FaceTermToInt(s2);
			std::vector<int> c3 = FaceTermToInt(s3);
			Triangle t = Triangle(c1[0] - 1, c2[0] - 1, c3[0] - 1);
			t.n1 = c1[2] - 1;
			t.n2 = c2[2] - 1;
			t.n3 = c3[2] - 1;
			t.u1 = c1[1] - 1;
			t.u2 = c2[1] - 1;
			t.u3 = c3[1] - 1;
			m_faces.push_back(t);
			if (iss >> s4)
			{
				std::vector<int> c4 = FaceTermToInt(s4);
				t = Triangle(c1[0] - 1, c3[0] - 1, c4[0] - 1);
				t.n1 = c1[2] - 1;
				t.n2 = c3[2] - 1;
				t.n3 = c4[2] - 1;
				t.u1 = c1[1] - 1;
				t.u2 = c3[1] - 1;
				t.u3 = c4[1] - 1;
				m_faces.push_back(t);
			}
		}
		else if (code == "vt")
		{
			iss >> u >> v;
			m_uv.push_back(glm::vec2(u, v));
		}
		else if (code == "vn")
		{
			iss >> nx >> ny >> nz;
			m_normals.push_back(glm::vec3(nx, ny, nz));
		}
	}
	glm::vec3 min = glm::vec3(min_x, min_y, min_z);
	glm::vec3 max = glm::vec3(max_x, max_y, max_z);
	m_bounding_volume = new Box(glm::vec3(0, 0, 0), 1.0);
	if (max_x == min_x)
	{
		max_x += 0.001;
	}
	if (max_y == min_y)
	{
		max_y += 0.001;
	}
	if (max_z == min_z)
	{
		max_z += 0.001;
	}
	glm::vec3 scale_p = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
	glm::mat4 T = glm::scale(glm::mat4(), scale_p);
	T = glm::translate(glm::mat4(), min) * T;
	m_norm_transform = glm::transpose(glm::inverse(glm::mat3(T)));
	m_transform = glm::inverse(T);
	for (auto &triangle : m_faces)
	{
		UpdateTriangle(triangle);
	}
}

Mesh::~Mesh()
{
	delete m_bounding_volume;
}

void Mesh::PhongInterpolate(const Triangle &tri, const glm::vec3 &point, glm::vec3 &normal, glm::vec2 &uv) const
{
	float alpha = TriangleArea(tri.vertices[1], tri.vertices[2], point);
	float beta = TriangleArea(tri.vertices[0], tri.vertices[2], point);
	float gamma = TriangleArea(tri.vertices[0], tri.vertices[1], point);
	glm::vec3 f = {alpha, beta, gamma};
	f /= tri.area;
	normal = f[0] * tri.normals[0] + f[1] * tri.normals[1] + f[2] * tri.normals[2];
	uv = f[0] * tri.uv[0] + f[1] * tri.uv[1] + f[2] * tri.uv[2];
}

bool Mesh::RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const
{
	const Ray scale_ray = Ray(glm::vec3(m_transform * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(m_transform * glm::vec4(ray.B, 1.0f)));
	double t_m = t_min;
	// Note this will make us ignore the entire object if the bounding box is close to or behind the
	// ray origin. This is okay with me.
	if (!m_bounding_volume->RayTest(scale_ray, t_m, normal, point, uv))
	{
		return false;
	}
	bool found = false;
	int idx = 0;
	for (size_t i = 0; i < m_faces.size(); ++i)
	{
		const Triangle &triangle = m_faces[i];
		glm::vec3 v1 = triangle.vertices[0];
		glm::vec3 v2 = triangle.vertices[1];
		glm::vec3 v3 = triangle.vertices[2];
		glm::vec3 A = v2 - v1;
		glm::vec3 B = v3 - v1;
		glm::vec3 C = -1.0f * ray.B_A;
		glm::vec3 R = ray.A - v1;
		float D = glm::determinant(glm::mat3(A, B, C));
		if (std::abs(D) == 0.0)
		{
			D = 0.0001;
		}
		float Dx = glm::determinant(glm::mat3(R, B, C));
		float Dy = glm::determinant(glm::mat3(A, R, C));
		float Dz = glm::determinant(glm::mat3(A, B, R));
		double beta = (double)Dx / D;
		double gamma = (double)Dy / D;
		if (beta < 0 || gamma < 0 || beta + gamma > 1)
		{
			continue;
		}
		double t = (double)Dz / D;
		if (t < t_min && t > 0.0001)
		{
			idx = i;
			t_min = t;
			found = true;
		}
	}
	if (found)
	{
		point = ray.GetPoint(t_min);
		PhongInterpolate(m_faces[idx], point, normal, uv);
		return true;
	}
	else
	{
		return false;
	}
};

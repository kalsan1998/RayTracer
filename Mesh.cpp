// Winter 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh(const std::string &fname)
	: m_vertices(), m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	double min_x = std::numeric_limits<double>::infinity();
	double min_y = min_x;
	double min_z = min_x;
	double max_x = -min_x;
	double max_y = -min_x;
	double max_z = -min_x;

	std::ifstream ifs(fname.c_str());
	while (ifs >> code)
	{
		if (code == "v")
		{
			ifs >> vx >> vy >> vz;
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
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
		}
	}
	glm::vec3 min = glm::vec3(min_x, min_y, min_z);
	glm::vec3 max = glm::vec3(max_x, max_y, max_z);
	bounding_volume = new NonhierBox(glm::vec3(0, 0, 0), 1.0);
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
	norm_transform = glm::transpose(glm::inverse(glm::mat3(T)));
	transform = glm::inverse(T);
}

Mesh::~Mesh()
{
	delete bounding_volume;
}

int Mesh::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
	const Ray scale_ray = Ray(glm::vec3(transform * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(transform * glm::vec4(ray.B, 1.0f)));
#ifdef RENDER_BOUNDING_VOLUMES
	return bounding_volume->Intersection(scale_ray, t_vals);
#endif
	if (bounding_volume->Intersection(scale_ray, t_vals, normal) < 1)
	{
		return 0;
	}
	double t_min = std::numeric_limits<double>::infinity();
	double t_max = 0.0;
	for (const Triangle &triangle : m_faces)
	{
		glm::vec3 v1 = m_vertices[triangle.v1];
		glm::vec3 v2 = m_vertices[triangle.v2];
		glm::vec3 v3 = m_vertices[triangle.v3];
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
		if (t < t_min && t > 0.0)
		{
#ifdef RENDER_BOUNDING_VOLUMES
			glm::vec3 p = ray.GetPoint(t_min);
			glm::vec3 t_p = glm::vec3(transform * glm::vec4(p, 1.0));
			return norm_transform * bounding_volume->Normal(t_p);
#endif
			normal = glm::cross(A, B);
			t_min = t;
		}
		t_max = std::max(t_max, t);
	}
	t_vals[0] = t_min;
	t_vals[1] = t_max;
	int count = 0;
	if (t_min < std::numeric_limits<double>::infinity())
	{
		++count;
	}
	if (t_max > 0.0)
	{
		++count;
	}
	return count;
};

std::ostream &operator<<(std::ostream &out, const Mesh &mesh)
{
	out << "mesh {";
	/*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
	out << "}";
	return out;
}

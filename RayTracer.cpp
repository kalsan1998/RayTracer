// Winter 2020

#include <glm/ext.hpp>
#include <vector>
#include <limits>
#include <math.h>

#include "RayTracer.hpp"
#include "GeometryNode.hpp"

Ray::Ray(const glm::vec3 &A, const glm::vec3 &B)
	: A(A), B(B), A_2(A * A), B_A(B-A), B_A_2(B_A * B_A)
{
}

glm::vec3 Ray::GetPoint(double t) const
{
	return A + t*B_A;
}

bool ObjectBetween(SceneNode *node, const Ray &ray, const glm::mat4 &m)
{
	glm::mat4 model = m * node->trans;
	for (SceneNode *child : node->children)
	{
		if (ObjectBetween(child, ray, model))
		{
			return true;
		}
	}
	if (node->m_nodeType != NodeType::GeometryNode)
	{
		return false;
	}
	GeometryNode *geo = static_cast<GeometryNode *>(node);
	double t_vals[2] = {0.0, 0.0};
	glm::mat4 inv_model = glm::inverse(model);
	const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
	if (geo->m_primitive->Intersection(ray_trans, t_vals) < 1)
	{
		return false;
	}
	if (std::min(t_vals[0], t_vals[1]) > 0.00001)
	{
		return true;
	}
	return false;
}

glm::vec3 Phong(
	SceneNode *root,
	const std::list<Light *> &lights,
	const glm::vec3 &norm,
	const glm::vec3 &norm_ray,
	const glm::vec3 &reflect,
	const glm::vec3 &point,
	const glm::vec3 &ambient,
	Material *material)
{
	glm::vec3 col = ambient;
	for (Light *light : lights)
	{
		glm::vec3 norm_pos = glm::normalize(light->position - point);
		if (ObjectBetween(root, Ray(point, light->position), glm::mat4()))
		{
			continue;
		}
		glm::vec3 diffuse = material->Diffuse() * std::max(glm::dot(norm_pos, norm), 0.0f);
		glm::vec3 specular = material->Specular() * pow(std::max(glm::dot(norm_pos, reflect), 0.0f), material->Shininess());
		double dist = glm::distance(light->position, point);
		double attenuation = light->falloff[0] + (light->falloff[1] * dist) + (light->falloff[2] * dist * dist);
		col += light->colour * (diffuse + specular) / (float)attenuation;
	}
	return col;
}

glm::mat4 GetTransform(
	double nx,
	double ny,
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double fovy)
{
	double d = 10.0;
	double h = 2.0 * d * tan(glm::radians(fovy / 2.0));
	double w = nx * h / ny;
	glm::mat4 transform;
	transform = glm::translate(glm::mat4(), glm::vec3(-nx / 2.0f, -ny / 2.0f, d));
	transform = glm::scale(glm::mat4(), glm::vec3(-h / ny, -w / nx, 1.0f)) * transform;
	glm::vec3 omega = glm::normalize(view);
	glm::vec3 mu = glm::normalize(glm::cross(up, omega));
	glm::vec3 v = glm::cross(omega, mu);
	glm::mat4 R = {
		glm::vec4(mu, 0.0),
		glm::vec4(v, 0.0),
		glm::vec4(omega, 0.0),
		glm::vec4(eye, 1.0)};
	transform = R * transform;
	return transform;
}

glm::mat3 NormTransform(const glm::mat4 &t)
{
	glm::mat3 m(t);
	return glm::inverse(glm::transpose(m));
}

bool Traverse(
	SceneNode *root,
	SceneNode *node,
	const Ray &ray,
	const std::list<Light *> &lights,
	const glm::vec3 &ambient,
	double &r,
	double &g,
	double &b,
	double &t_min,
	const glm::mat4 &trans,
	int hits)
{
	const glm::mat4 model = trans * node->trans;

	bool hit = false;
	for (SceneNode *child : node->children)
	{
		hit |= Traverse(root, child, ray, lights, ambient, r, g, b, t_min, model, hits);
	}
	if (node->m_nodeType != NodeType::GeometryNode)
	{
		return hit;
	}
	GeometryNode *geo = static_cast<GeometryNode *>(node);
	double t_vals[2] = {0.0, 0.0};

	glm::mat4 inv_model = glm::inverse(model);
	const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
	if (geo->m_primitive->Intersection(ray_trans, t_vals) < 2)
	{
		return hit;
	}
	double mt = std::min(t_vals[0], t_vals[1]);
	if (t_min <= mt || mt <= 0.0)
	{
		return hit;
	}
	t_min = mt;
	glm::vec3 point = ray_trans.GetPoint(t_min);
	glm::vec3 world_point = glm::vec3(model * glm::vec4(point, 1.0f));
	glm::vec3 kd = geo->m_material->Diffuse();
	glm::vec3 normal = NormTransform(model) * geo->m_primitive->Normal(point);

	glm::vec3 norm = glm::normalize(normal);
	glm::vec3 norm_ray = glm::normalize(ray.B_A);
	glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, norm) * norm));
	glm::vec3 phong = Phong(root, lights, norm, norm_ray, reflect, world_point, ambient, geo->m_material);
	double reflectivity = geo->m_material->Reflectivity();
	if (reflectivity && hits < 5)
	{
		double rr = 0.1;
		double gg = 0.1;
		double bb = 0.5;
		double tt_min = std::numeric_limits<double>::max();
		Ray reflect_ray(world_point, world_point + reflect);
		Traverse(root, root, reflect_ray, lights, ambient, rr, gg, bb, tt_min, glm::mat4(), hits + 1);
		r = (reflectivity * rr) + ((1.0 - reflectivity) * phong[0]);
		g = (reflectivity * gg) + ((1.0 - reflectivity) * phong[1]);
		b = (reflectivity * bb) + ((1.0 - reflectivity) * phong[2]);
	}
	else
	{
		r = phong[0];
		g = phong[1];
		b = phong[2];
	}

	return true;
}

void A4_Render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height  2.0f *
	Image &image,

	// Viewing parameters
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double fovy,

	// Lighting parameters
	const glm::vec3 &ambient,
	const std::list<Light *> &lights)
{

	// Fill in raytracing code here...

	std::cout << "Calling A4_Render" << std::endl;

	size_t nx = image.height();
	size_t ny = image.width();
	glm::mat4 transform = GetTransform(nx, ny, eye, view, up, fovy);

	int ten_p = ny / 20 + 1;
	int n = 0;

	for (uint y = 0; y < ny; ++y)
	{
		if (y == n * ten_p)
		{
			std::cout << "Progress: " << n++ * 5 << "\%\r";
			std::cout.flush();
		}
		for (uint x = 0; x < nx; ++x)
		{
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			double p_b = 0.9 * (y + x) / (ny + nx);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					glm::vec4 p = {(double)x + (double)i / 3, (double)y + (double)j / 3, 0.0, 1.0f};
					glm::vec3 p_world = glm::vec3(transform * p);
					Ray ray(eye, p_world);
					double t_min = std::numeric_limits<double>::max();
					double rr, gg, bb;
					if (!Traverse(root, root, ray, lights, ambient, rr, gg, bb, t_min, glm::mat4(), 0))
					{
						r += 0.1;
						g += 0.1;
						b += 0.1 + p_b;
					}
					else
					{
						r += rr;
						g += gg;
						b += bb;
					}
				}
			}
			r = r / 9.0;
			g = g / 9.0;
			b = b / 9.0;

			// Red:
			image(x, y, 0) = r;
			// Green:
			image(x, y, 1) = g;
			// Blue:
			image(x, y, 2) = b;
		}
	}
	std::cout << "Progress: 100\%" << std::endl;
}

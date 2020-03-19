// Winter 2020

#include <glm/ext.hpp>
#include <vector>
#include <limits>
#include <math.h>
#include <thread>

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include "SurfaceInteraction.hpp"

const int THREAD_COUNT = 12;
const int ROWS_PER_THREAD = 10;

const double MAX_COLOR_DIFF = 0.1;

Ray::Ray(const glm::vec3 &A, const glm::vec3 &B)
	: A(A), B(B), A_2(A * A), B_A(B - A), B_A_2(B_A * B_A)
{
}

glm::vec3 Ray::GetPoint(double t) const
{
	return A + t * B_A;
}

bool ObjectBetween(const SceneNode *node, const Ray &ray, const glm::mat4 &m)
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
	const GeometryNode *geo = static_cast<const GeometryNode *>(node);
	double t_vals[2] = {0.0, 0.0};
	glm::mat4 inv_model = glm::inverse(model);
	const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
	glm::vec3 norm;
	if (geo->m_primitive->Intersection(ray_trans, t_vals, norm) < 2)
	{
		return false;
	}
	if (t_vals[0] > 0.0001 || t_vals[1] > 0.0001)
	{
		return true;
	}
	return false;
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
	const SceneNode *root,
	const SceneNode *node,
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

	bool did_hit = false;
	for (SceneNode *child : node->children)
	{
		did_hit |= Traverse(root, child, ray, lights, ambient, r, g, b, t_min, model, hits);
	}
	if (node->m_nodeType != NodeType::GeometryNode)
	{
		return did_hit;
	}
	const GeometryNode *geo = static_cast<const GeometryNode *>(node);
	double t_vals[2] = {0.0, 0.0};

	glm::mat4 inv_model = glm::inverse(model);
	const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
	glm::vec3 normal;
	if (geo->m_primitive->Intersection(ray_trans, t_vals, normal) < 2)
	{
		return did_hit;
	}
	double mt = std::min(t_vals[0], t_vals[1]);
	if (t_min <= mt || mt <= 0.001)
	{
		return did_hit;
	}
	t_min = mt;
	glm::vec3 point = ray_trans.GetPoint(t_min);
	glm::vec3 world_point = glm::vec3(model * glm::vec4(point, 1.0f));
	glm::vec3 kd = geo->m_material->Diffuse();
	normal = NormTransform(model) * normal;

	normal = glm::normalize(normal);
	glm::vec3 norm_ray = glm::normalize(ray.B_A);
	glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, normal) * normal));
	glm::vec3 phong = Phong(root, lights, normal, norm_ray, reflect, world_point, ambient, geo->m_material);
	double reflectivity = geo->m_material->Reflectivity();
	double refractivity = geo->m_material->Refractivity();
	double ior = geo->m_material->IndexOfRefraction();
	double refl_col[3] = {0.1, 0.1, 0.5};
	double refr_col[3] = {0.1, 0.1, 0.5};
	if (reflectivity && hits < 5)
	{
		double tt_min = std::numeric_limits<double>::max();
		Ray reflect_ray(world_point, world_point + reflect);
		Traverse(root, root, reflect_ray, lights, ambient, refl_col[0], refl_col[1], refl_col[2], tt_min, glm::mat4(), hits + 1);
	}
	if (refractivity && hits < 5)
	{
		double tt_min = std::numeric_limits<double>::max();
		Ray refract_ray = Snells(world_point, norm_ray, normal, ior);
		if (refract_ray.B != world_point)
		{
			Traverse(root, root, refract_ray, lights, ambient, refr_col[0], refr_col[1], refr_col[2], tt_min, glm::mat4(), hits + 1);
		}
	}
	double opa = 1.0 - reflectivity - refractivity;
	r = (refractivity * refr_col[0]) + (reflectivity * refl_col[0]) + (opa * phong[0]);
	g = (refractivity * refr_col[1]) + (reflectivity * refl_col[1]) + (opa * phong[1]);
	b = (refractivity * refr_col[2]) + (reflectivity * refl_col[2]) + (opa * phong[2]);

	return true;
}

struct RowRenderArgs
{
	const SceneNode *root;
	Image *image;
	const glm::vec3 *eye;
	const glm::vec3 *ambient;
	const std::list<Light *> *lights;
	const glm::mat4 *transform;
	size_t nx;
	size_t ny;
	size_t y;
	Image *post_image; // For anti-aliasing
};

void Render_Row(void *data)
{
	struct RowRenderArgs *args = (struct RowRenderArgs *)data;
	Image &image = *args->image;
	size_t max_y = std::min(args->ny, args->y + ROWS_PER_THREAD);
	for (uint y = args->y; y < max_y; ++y)
	{
		for (uint x = 0; x < args->nx; ++x)
		{
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;

			double p_b = 0.9 * (y + x) / (args->ny + args->nx);
			glm::vec4 p = {(double)x, (double)y, 0.0, 1.0f};
			glm::vec3 p_world = glm::vec3(*args->transform * p);
			Ray ray(*args->eye, p_world);
			double t_min = std::numeric_limits<double>::max();
			if (!Traverse(args->root, args->root, ray, *args->lights, *args->ambient, r, g, b, t_min, glm::mat4(), 0))
			{
				r = 0.1;
				g = 0.1;
				b = 0.1 + p_b;
			}
			image(x, y, 0) = r;
			image(x, y, 1) = g;
			image(x, y, 2) = b;
		}
	}
	return;
}

void AntiAlias(void *data)
{

	double pairs[5][2] = {
		{0, 0.5}, {0.5, 0}, {0.5, 0.5}, {0.5, 1}, {1, 0.5}};
	struct RowRenderArgs *args = (struct RowRenderArgs *)data;
	Image &image = *args->post_image;
	Image &pre_image = *args->image;
	size_t max_y = std::min(args->ny - 1, args->y + ROWS_PER_THREAD);
	for (uint y = args->y; y < max_y; ++y)
	{
		for (uint x = 0; x < args->nx - 1; ++x)
		{
			double p_b = 0.9 * (y + x) / (args->ny + args->nx);
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			double min_r = 1.0;
			double min_g = 1.0;
			double min_b = 1.0;
			double max_r = 0.0;
			double max_g = 0.0;
			double max_b = 0.0;
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					double rr = std::min(1.0, pre_image(x + i, y + j, 0));
					double gg = std::min(1.0, pre_image(x + i, y + j, 1));
					double bb = std::min(1.0, pre_image(x + i, y + j, 2));
					r += rr;
					g += gg;
					b += bb;
					min_r = std::min(min_r, rr);
					min_g = std::min(min_g, gg);
					min_b = std::min(min_b, bb);
					max_r = std::max(max_r, rr);
					max_g = std::max(max_g, gg);
					max_b = std::max(max_b, bb);
				}
			}
			double diff = ((max_r + max_g + max_b) - (min_r + min_g + min_b)) / 3.0;
			if (max_r - min_r > MAX_COLOR_DIFF || max_g - min_g > MAX_COLOR_DIFF || max_b - min_b > MAX_COLOR_DIFF || diff > MAX_COLOR_DIFF)
			{
				for (int i = 0; i < 5; ++i)
				{
					glm::vec4 p = {(double)x + pairs[i][0], (double)y + pairs[i][0], 0.0, 1.0f};
					glm::vec3 p_world = glm::vec3(*args->transform * p);
					Ray ray(*args->eye, p_world);
					double t_min = std::numeric_limits<double>::max();
					double rr, gg, bb;
					if (!Traverse(args->root, args->root, ray, *args->lights, *args->ambient, rr, gg, bb, t_min, glm::mat4(), 0))
					{
						rr = 0.1;
						gg = 0.1;
						bb = 0.1 + p_b;
					}
					r += rr;
					g += gg;
					b += bb;
				}
				r = r / 9.0;
				g = g / 9.0;
				b = b / 9.0;
			}
			else
			{
				r = r / 4.0;
				g = g / 4.0;
				b = b / 4.0;
			}
			image(x, y, 0) = r;
			image(x, y, 1) = g;
			image(x, y, 2) = b;
		}
	}
	return;
}

void Render(
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
	time_t time1;
	time(&time1);
	const size_t nx = image.height();
	const size_t ny = image.width();
	const glm::mat4 transform = GetTransform(nx, ny, eye, view, up, fovy);
	Image pre_image = image;

	int n = 0;
	int p = 100 * THREAD_COUNT * ROWS_PER_THREAD / ny;
	// Initialize threads
	std::vector<std::thread> threads(THREAD_COUNT);
	struct RowRenderArgs default_args = {root, &pre_image, &eye, &ambient, &lights, &transform, nx, ny, 0, &image};
	std::vector<struct RowRenderArgs> args = std::vector<struct RowRenderArgs>(THREAD_COUNT);
	for (auto &arg : args)
	{
		arg = default_args;
	}

	for (uint y = 0; y < ny; y += (THREAD_COUNT * ROWS_PER_THREAD))
	{
		std::cout << "Progress: " << n << "\%\r";
		std::cout.flush();
		n += p;

		int offset = 0;
		for (int i = 0; i < THREAD_COUNT; ++i)
		{
			args[i].y = y + offset;
			offset += ROWS_PER_THREAD;
			threads[i] = std::thread(Render_Row, &args[i]);
		}
		for (auto &th : threads)
		{
			th.join();
		}
	}
	std::cout << "Progress: 100\%" << std::endl;

	n = 0;
	for (uint y = 0; y < ny; y += (THREAD_COUNT * ROWS_PER_THREAD))
	{
		std::cout << "Anti-Aliasing Progress: " << n << "\%\r";
		std::cout.flush();
		n += p;

		int offset = 0;
		for (int i = 0; i < THREAD_COUNT; ++i)
		{
			args[i].y = y + offset;
			offset += ROWS_PER_THREAD;
			threads[i] = std::thread(AntiAlias, &args[i]);
		}
		for (auto &th : threads)
		{
			th.join();
		}
	}
	std::cout << "Anti-Aliasing Progress: 100\%" << std::endl;

	time_t time2;
	time(&time2);
	std::cout << "Time (s): " << difftime(time2, time1) << std::endl;
}

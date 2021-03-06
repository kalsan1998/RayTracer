// Winter 2020

#include <glm/ext.hpp>
#include <vector>
#include <limits>
#include <math.h>
#include <thread>

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include "SurfaceInteraction.hpp"
#include "Texture.hpp"
#include "PhotonMapping.hpp"
#include "KdTree.hpp"

const int RENDER_THREAD_COUNT = 12;
const int ROWS_PER_THREAD = 10;

const int MAX_HITS = 5;
const double MAX_COLOR_DIFF = 0.1;

Ray::Ray(const glm::vec3 &A, const glm::vec3 &B)
	: A(A), B(B), A_2(A * A), B_A(B - A), B_A_2(B_A * B_A)
{
}

glm::vec3 Ray::GetPoint(double t) const
{
	return A + t * B_A;
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
	int hits,
	const PhotonMap &global_map,
	const PhotonMap &caustic_map)
{
	const glm::mat4 model = trans * node->trans;
	bool did_hit = false;
	for (SceneNode *child : node->children)
	{
		did_hit |= Traverse(root, child, ray, lights, ambient, r, g, b, t_min, model, hits, global_map, caustic_map);
	}
	if (node->m_nodeType != NodeType::GeometryNode)
	{
		return did_hit;
	}
	const GeometryNode *geo = static_cast<const GeometryNode *>(node);

	glm::mat4 inv_model = glm::inverse(model);
	const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
							  glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
	glm::vec3 normal;
	glm::vec3 point;
	glm::vec2 uv;
	if (!geo->m_primitive->RayTest(ray_trans, t_min, normal, point, uv))
	{
		return did_hit;
	}
	glm::vec3 object_color = geo->PointColor(uv);
	glm::vec3 world_point = glm::vec3(model * glm::vec4(point, 1.0f));
	glm::vec3 kd = geo->m_material->Diffuse();
	normal = NormTransform(model) * normal;

	normal = glm::normalize(normal);
	glm::vec3 norm_ray = glm::normalize(ray.B_A);
	glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, normal) * normal));
	glm::vec3 phong = Colour(root, lights, normal, norm_ray, reflect, world_point, ambient, object_color, geo->m_material, global_map, caustic_map);
	double reflectivity = geo->m_material->Reflectivity();
	double refractivity = geo->m_material->Refractivity();
	double ior = geo->m_material->IndexOfRefraction();
	double refl_col[3] = {0.0, 0.0, 0.0};
	double refr_col[3] = {0.0, 0.0, 0.0};
	Ray refract_ray(glm::vec3(0), glm::vec3(0));
	if (refractivity && hits < MAX_HITS)
	{
		double tt_min = std::numeric_limits<double>::max();
		refract_ray = CalculateRefraction(world_point, norm_ray, normal, ior, reflectivity);
		reflectivity *= refractivity;
		refractivity -= reflectivity;
		if (refract_ray.B != world_point)
		{
			Traverse(root, root, refract_ray, lights, ambient, refr_col[0], refr_col[1], refr_col[2], tt_min, glm::mat4(), hits + 1, global_map, caustic_map);
		}
	}
	if (reflectivity && hits < MAX_HITS)
	{
		double tt_min = std::numeric_limits<double>::max();
		Ray reflect_ray(world_point, world_point + reflect);
		Traverse(root, root, reflect_ray, lights, ambient, refl_col[0], refl_col[1], refl_col[2], tt_min, glm::mat4(), hits + 1, global_map, caustic_map);
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
	const PhotonMap *global_map;
	const PhotonMap *caustic_map;
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

			glm::vec4 p = {(double)x, (double)y, 0.0, 1.0f};
			glm::vec3 p_world = glm::vec3(*args->transform * p);
			Ray ray(*args->eye, p_world);
			double t_min = std::numeric_limits<double>::max();
			Traverse(args->root, args->root, ray, *args->lights, *args->ambient, r, g, b, t_min, glm::mat4(), 0, *args->global_map, *args->caustic_map);
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
					if (!Traverse(args->root, args->root, ray, *args->lights, *args->ambient, rr, gg, bb, t_min, glm::mat4(), 0, *args->global_map, *args->caustic_map))
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

	PhotonMapper photon_mapper;
	photon_mapper.MapPhotons(lights, root);

	const PhotonMap &global_map = *photon_mapper.GlobalMap();
	const PhotonMap &caustic_map = *photon_mapper.CausticMap();

	std::cout << "Global photon map size: " << global_map.size() << std::endl;
	std::cout << "Caustic photon map size: " << caustic_map.size() << std::endl;

	int n = 0;
	int p = 100 * RENDER_THREAD_COUNT * ROWS_PER_THREAD / ny;
	// Initialize threads
	std::vector<std::thread> threads(RENDER_THREAD_COUNT);
	struct RowRenderArgs default_args = {root, &pre_image, &eye, &ambient, &lights, &transform, nx, ny, 0, &image, &global_map, &caustic_map};
	std::vector<struct RowRenderArgs> args = std::vector<struct RowRenderArgs>(RENDER_THREAD_COUNT);
	for (auto &arg : args)
	{
		arg = default_args;
	}

	for (uint y = 0; y < ny; y += (RENDER_THREAD_COUNT * ROWS_PER_THREAD))
	{
		std::cout << "Ray Trace Progress: " << n << "\%\r";
		std::cout.flush();
		n += p;

		int offset = 0;
		for (int i = 0; i < RENDER_THREAD_COUNT; ++i)
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
	std::cout << "Ray Trace Progress: 100\%" << std::endl;

	n = 0;
	for (uint y = 0; y < ny; y += (RENDER_THREAD_COUNT * ROWS_PER_THREAD))
	{
		std::cout << "Anti-Aliasing Progress: " << n << "\%\r";
		std::cout.flush();
		n += p;

		int offset = 0;
		for (int i = 0; i < RENDER_THREAD_COUNT; ++i)
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

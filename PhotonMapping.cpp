#include "PhotonMapping.hpp"

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include "SurfaceInteraction.hpp"

#include <glm/ext.hpp>

// Photons per light source.
const uint PHOTONS_PER_THREAD = 10000;
const uint PHOTON_MAPPING_THREAD_COUNT = 10;

// Returns the node that is *closer* (using the t_min value)
const GeometryNode *GetClosestIntersection(const SceneNode *node, const Ray &ray, const glm::mat4 trans,
                                           double &t_min, glm::vec3 &normal, glm::vec3 &point)
{
    const GeometryNode *closest = nullptr;
    const glm::mat4 model = trans * node->trans;
    for (SceneNode *child : node->children)
    {
        const GeometryNode *c = GetClosestIntersection(child, ray, model, t_min, normal, point);
        if (c)
        {
            closest = c;
        }
    }
    if (node->m_nodeType != NodeType::GeometryNode)
    {
        return closest;
    }
    const GeometryNode *geo = static_cast<const GeometryNode *>(node);
    glm::mat4 inv_model = glm::inverse(model);
    const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
                              glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
    glm::vec2 uv;
    glm::vec3 pp;
    if (!geo->m_primitive->RayTest(ray_trans, t_min, normal, pp, uv))
    {
        return closest;
    }
    point = glm::vec3(model * glm::vec4(pp, 1.0f));
    normal = glm::inverse(glm::transpose(glm::mat3(model))) * normal;
    normal = glm::normalize(normal);
    return geo;
}

PhotonMapper::PhotonMapper() : global(),
                               caustic(),
                               threads(PHOTON_MAPPING_THREAD_COUNT),
                               global_lock(),
                               caustic_lock(),
                               progress(0.f),
                               progress_lock()
{
}

void PhotonMapper::MapPhotons(const std::list<Light *> &lights, const SceneNode *root)
{

    std::cout << "Mapping photons for " << lights.size() << " light sources." << std::endl;
    for (auto *light : lights)
    {
        for (uint t = 0; t < PHOTON_MAPPING_THREAD_COUNT; ++t)
        {
            threads[t] = std::thread(&PhotonMapper::EmitPhotonBatch, this, light, root);
        }
        for (auto &th : threads)
        {
            th.join();
        }
        std::cout << "Photon Mapping Progress: 100\%" << std::endl;
    }
}

void PhotonMapper::EmitPhotonBatch(const Light *light, const SceneNode *root)
{
    glm::vec3 power = light->colour;
    int n = 0;
    for (uint i = 0; i < PHOTONS_PER_THREAD; ++i)
    {
        if (++n > 1000)
        {
            update_progress(n);
            n = 0;
        }
        Photon photon;
        photon.power = power;

        float x = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
        float y = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
        float z = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
        if (x == 0 && y == 0 && z == 0)
        {
            continue; // Lazy!
        }
        glm::vec3 direction = {x, y, z};
        direction = glm::normalize(direction);
        Ray emit = Ray(light->position, light->position + direction);
        EmitPhoton(root, &emit, photon);
    }
}

void PhotonMapper::EmitPhoton(const SceneNode *root, const Ray *ray, Photon &photon)
{
    double t_min = std::numeric_limits<double>::max();
    glm::vec3 world_point;
    glm::vec3 point;
    glm::vec3 normal;
    const GeometryNode *surface = GetClosestIntersection(root, *ray, glm::mat4(), t_min, normal, world_point);
    if (!surface)
    {
        return;
    }
    glm::vec3 norm_ray = glm::normalize(ray->B_A);
    Ray ray_out(glm::vec3(0), glm::vec3(0));
    double reflectivity = surface->m_material->Reflectivity();
    double refractivity = surface->m_material->Refractivity();
    glm::vec3 diffuse = surface->m_material->Diffuse();
    double ior = surface->m_material->IndexOfRefraction();
    if (refractivity)
    {
        ray_out = CalculateRefraction(world_point, norm_ray, normal, ior, reflectivity);
        reflectivity *= refractivity;
        refractivity -= reflectivity;
    }
    float P_r = std::max(std::max(diffuse[0], diffuse[1]), diffuse[2]);
    float P_d = (1. - refractivity - reflectivity) * P_r;
    float P_refr = refractivity * P_r;
    float P_refl = reflectivity * P_r;
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    photon.power *= diffuse;

    if (r < P_refl)
    {
        // Specular Reflect
        glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, normal) * normal));
        ray_out = Ray(world_point, world_point + reflect);
    }
    else if (r < P_refl + P_refr)
    {
        // Refraction
        // NOTE: might hit itself
        // Already calculated refract ray
    }
    else if (r < P_refl + P_refr + P_d)
    {
        if (photon.caustic && photon.first_surface_hit)
        {
            insert_caustic(photon, world_point);
        }
        // Diffuse
        photon.caustic = false;
        glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, normal) * normal));
        ray_out = Ray(world_point, world_point + reflect);
    }
    else if (photon.first_surface_hit)
    {
        if (photon.caustic)
        {
            insert_caustic(photon, world_point);
        }
        insert_global(photon, world_point);
        return;
    }
    else
    {
        return;
    }
    photon.first_surface_hit = true;
    EmitPhoton(root, &ray_out, photon);
}

void PhotonMapper::insert_global(Photon &photon, glm::vec3 &point)
{
    global_lock.lock();
    global.insert(photon, point);
    global_lock.unlock();
}
void PhotonMapper::insert_caustic(Photon &photon, glm::vec3 &point)
{
    caustic_lock.lock();
    caustic.insert(photon, point);
    caustic_lock.unlock();
}

void PhotonMapper::update_progress(int n)
{
    progress_lock.lock();
    progress += n;
    std::cout << "Photon Mapping Progress: " << 100 * progress / (PHOTONS_PER_THREAD * PHOTON_MAPPING_THREAD_COUNT) << "\%\r";
    std::cout.flush();
    progress_lock.unlock();
}

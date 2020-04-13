#include "PhotonMapping.hpp"

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include "SurfaceInteraction.hpp"

// Photons per light source.
const uint NUM_PHOTONS = 400000;

const GeometryNode *GetClosestIntersection(const SceneNode *node, const Ray &ray, const glm::mat4 trans,
                                           double &t_min, glm::vec3 &normal, glm::vec3 point)
{
    const GeometryNode *closest = nullptr;
    const glm::mat4 model = trans * node->trans;
    for (SceneNode *child : node->children)
    {
        closest = GetClosestIntersection(child, ray, model, t_min, normal, point);
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

void EmitPhoton(const SceneNode *root, const Ray &ray, Photon &photon,
                PhotonMap *global, PhotonMap *caustic)
{
    double t_min = std::numeric_limits<double>::max();
    glm::vec3 world_point;
    glm::vec3 point;
    glm::vec3 normal;
    const GeometryNode *surface = GetClosestIntersection(root, ray, glm::mat4(), t_min, normal, world_point);
    if (!surface)
    {
        return;
    }
    glm::vec3 norm_ray = glm::normalize(ray.B_A);
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
    photon.power *= diffuse / P_r;
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
        // Diffuse
        photon.caustic = false;
        glm::vec3 reflect = norm_ray - (2.0f * (glm::dot(norm_ray, normal) * normal));
        ray_out = Ray(world_point, world_point + reflect);
    }
    else if (!photon.first_surface_hit)
    {
        if (photon.caustic)
        {
            caustic->insert(photon, world_point);
        }
        global->insert(photon, world_point);
    }
    photon.first_surface_hit = true;
    EmitPhoton(root, ray_out, photon, global, caustic);
}

void MapPhotons(const std::list<Light *> &lights, const SceneNode *root,
                PhotonMap *global, PhotonMap *caustic)
{
    // TODO ADD PROGRESS OUTPUT + MULTITHREAD
    for (auto *light : lights)
    {
        Photon photon;
        photon.power = light->colour / (float)NUM_PHOTONS;

        double phi = 0.0;
        double theta = 0.0;
        for (uint i = 0; i < NUM_PHOTONS; ++i)
        {
            std::cout << "Progress: " << 100. * i / (float)NUM_PHOTONS << "%\r";
            std::cout.flush();

            int x = rand();
            int y = rand();
            int z = rand();
            if (x == 0 && y == 0 && z == 0)
            {
                continue; // Lazy!
            }
            glm::vec3 direction = {x, y, z};
            direction = glm::normalize(direction);
            Ray emit = Ray(light->position, direction);
            EmitPhoton(root, emit, photon, global, caustic);
        }
    }
}

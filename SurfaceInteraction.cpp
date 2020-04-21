#include "SurfaceInteraction.hpp"

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include "KdTree.hpp"

#include <glm/ext.hpp>

bool LightReached(const SceneNode *node, const Ray &ray, const glm::mat4 &m, const double &t_min)
{
    glm::mat4 model = m * node->trans;
    for (SceneNode *child : node->children)
    {
        bool l = LightReached(child, ray, model, t_min);
        if (!l)
        {
            return false;
        }
    }
    if (node->m_nodeType != NodeType::GeometryNode)
    {
        return true;
    }
    const GeometryNode *geo = static_cast<const GeometryNode *>(node);
    double t_vals[2] = {0.0, 0.0};
    glm::mat4 inv_model = glm::inverse(model);
    const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
                              glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
    glm::vec3 normal;
    glm::vec3 point;
    glm::vec2 uv;
    double t_mint = t_min;
    if (!geo->m_primitive->RayTest(ray_trans, t_mint, normal, point, uv))
    {
        return true;
    }
    else
    {
        return false;
    }
}

glm::vec3 Colour(
    const SceneNode *root,
    const std::list<Light *> &lights,
    const glm::vec3 &norm,
    const glm::vec3 &norm_ray,
    const glm::vec3 &reflect,
    const glm::vec3 &point,
    const glm::vec3 &ambient,
    const glm::vec3 &object_color,
    Material *material,
    const PhotonMap &global_map,
    const PhotonMap &caustic_map)
{
    float max_distance;
    glm::vec3 global = {0, 0, 0};
    glm::vec3 caustic = {0, 0, 0};

    std::vector<Photon *> photons = global_map.k_nearest(1, point, max_distance);
    // for (auto *photon : photons)
    // {
    //     global += photon->power;
    // }
    // global /= M_PI * max_distance * max_distance * 100.f;

    std::vector<Photon *> caustic_photons = caustic_map.k_nearest(500, point, max_distance);
    for (auto *photon : caustic_photons)
    {
        caustic += photon->power;
    }
    caustic /= M_PI * max_distance * max_distance * 500.f;
    glm::vec3 m = caustic;

    // glm::vec3 m = (global + caustic) / 2.0f;
    glm::vec3 col = m * object_color;
    // glm::vec3 col = ambient * object_color;
    for (Light *light : lights)
    {
        double l = LightReached(root, Ray(point, light->position), glm::mat4(), 1.0);
        if (l == 0.0)
        {
            continue;
        }
        glm::vec3 norm_pos = glm::normalize(light->position - point);
        glm::vec3 diffuse = object_color * std::max(glm::dot(norm_pos, norm), 0.0f);
        glm::vec3 specular = material->Specular() * pow(std::max(glm::dot(norm_pos, reflect), 0.0f), material->Shininess());
        double dist = glm::distance(light->position, point);
        double attenuation = light->falloff[0] + (light->falloff[1] * dist) + (light->falloff[2] * dist * dist);
        col += (float)l * (light->colour * (diffuse + specular) / (float)attenuation);
    }
    return col;
}

Ray CalculateRefraction(
    const glm::vec3 &world_point,
    const glm::vec3 &incident,
    const glm::vec3 &normal,
    double ior,
    double &reflect_amount)
{
    glm::vec3 N = normal;
    double cosi = glm::dot(incident, N);
    double etai = 1.0f; //air
    double etat = ior;
    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        N = -N;
    }
    double sint = etai / etat * sqrtf(std::max(0.0, 1.0 - cosi * cosi));
    if (sint >= 1)
    {
        reflect_amount = 1;
    }
    else
    {
        double cost = sqrtf(std::max(0.0, 1.0 - sint * sint));
        double ab_cosi = std::abs(cosi);
        double etci = etat * ab_cosi;
        double eici = etai * ab_cosi;
        double etct = etat * cost;
        double eict = etai * cost;
        double Rs = (etci - eict) / (etci + eict);
        double Rp = (eici - etct) / (eici + etct);
        reflect_amount = (Rs * Rs + Rp * Rp) / 2.0;
    }
    double eta = etai / etat;
    double k = 1.0 - eta * eta * (1.0 - cosi * cosi);
    // k = 0 => Total internal reflection
    return k < 0 ? Ray(world_point, world_point)
                 : Ray(world_point, world_point + ((float)eta * incident + (float)(eta * cosi - sqrt(k)) * N));
}
#include "SurfaceInteraction.hpp"

#include "RayTracer.hpp"
#include "GeometryNode.hpp"
#include <glm/ext.hpp>

double LightReached(const SceneNode *node, const Ray &ray, const glm::mat4 &m)
{
    double reached = 1.0;
    glm::mat4 model = m * node->trans;
    for (SceneNode *child : node->children)
    {
        double l = LightReached(child, ray, model);
        if (l)
        {
            reached *= l;
        }
        else
        {
            return 0.0;
        }
    }
    if (node->m_nodeType != NodeType::GeometryNode)
    {
        return 1.0;
    }
    const GeometryNode *geo = static_cast<const GeometryNode *>(node);
    double t_vals[2] = {0.0, 0.0};
    glm::mat4 inv_model = glm::inverse(model);
    const Ray ray_trans = Ray(glm::vec3(inv_model * glm::vec4(ray.A, 1.0f)),
                              glm::vec3(inv_model * glm::vec4(ray.B, 1.0f)));
    glm::vec3 norm;
    if (geo->m_primitive->Intersection(ray_trans, t_vals, norm) < 1)
    {
        return reached;
    }
    if (t_vals[0] > 0.0001 && t_vals[1] > 0.0001)
    {
        return reached * geo->m_material->Refractivity();
    }
    return reached;
}

glm::vec3 Phong(
    const SceneNode *root,
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
        double l = LightReached(root, Ray(point, light->position), glm::mat4());
        if (!l)
        {
            continue;
        }
        glm::vec3 diffuse = material->Diffuse() * std::max(glm::dot(norm_pos, norm), 0.0f);
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
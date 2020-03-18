#include "Illumination.hpp"

#include "RayTracer.hpp"
#include <glm/ext.hpp>

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
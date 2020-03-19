#pragma once

#include <glm/glm.hpp>
#include <list>

class SceneNode;
class Light;
class Material;
class Ray;

glm::vec3 Phong(
    const SceneNode *root,
    const std::list<Light *> &lights,
    const glm::vec3 &norm,
    const glm::vec3 &norm_ray,
    const glm::vec3 &reflect,
    const glm::vec3 &point,
    const glm::vec3 &ambient,
    Material *material);

Ray CalculateRefraction(
    const glm::vec3 &world_point,
    const glm::vec3 &incident,
    const glm::vec3 &normal,
    double ior,
    double &reflect_amount);

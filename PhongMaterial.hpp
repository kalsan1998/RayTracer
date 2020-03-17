// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material
{
public:
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflectivity);
  virtual ~PhongMaterial();
  glm::vec3 Diffuse() const override { return m_kd; }
  glm::vec3 Specular() const override { return m_ks; }
  double Shininess() const override { return m_shininess; }
  double Reflectivity() const override { return m_reflectivity; }

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflectivity;
};

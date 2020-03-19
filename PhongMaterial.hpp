// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

/*
  Reflectivity and Refractivity are coefficients for blending the color with Phong.
*/
class PhongMaterial : public Material
{
public:
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflectivity, double refractivity, double ior);
  virtual ~PhongMaterial();
  glm::vec3 Diffuse() const override { return m_kd; }
  glm::vec3 Specular() const override { return m_ks; }
  double Shininess() const override { return m_shininess; }
  double Reflectivity() const override { return m_reflectivity; }
  double Refractivity() const override { return m_refractivity; }
  double IndexOfRefraction() const override { return m_ior; }

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflectivity;
  double m_refractivity;
  double m_ior;
};

class ReflectiveMaterial : public PhongMaterial
{
public:
  ReflectiveMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflectivity);
  virtual ~ReflectiveMaterial();
};

class RefractiveMaterial : public PhongMaterial
{
public:
  RefractiveMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double refractivity, double ior);
  virtual ~RefractiveMaterial();
};

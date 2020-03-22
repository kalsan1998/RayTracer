// Winter 2020

#pragma once

#include <glm/glm.hpp>

class Material
{
public:
  virtual ~Material();
  virtual glm::vec3 Diffuse() const { return glm::vec3(0.0f); }
  virtual glm::vec3 Specular() const { return glm::vec3(0.0f); }
  virtual double Shininess() const { return 0.0; }
  virtual double Reflectivity() const { return 0.0; }
  virtual double Refractivity() const { return 0.0; }
  virtual double IndexOfRefraction() const { return 0.0; }

  Material();
};

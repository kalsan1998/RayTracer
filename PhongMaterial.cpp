// Winter 2020

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflectivity, double refractivity, double ior)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), m_reflectivity(reflectivity),
	  m_refractivity(refractivity), m_ior(ior)
{
}

PhongMaterial::~PhongMaterial()
{
}

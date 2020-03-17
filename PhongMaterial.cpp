// Winter 2020

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflectivity)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), m_reflectivity(reflectivity)
{
}

PhongMaterial::~PhongMaterial()
{
}

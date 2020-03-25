#pragma once

#include <vector>

#include "SceneNode.hpp"
#include "Primitive.hpp"

class Material;
class Texture;

class GeometryNode : public SceneNode
{
public:
	GeometryNode(const std::string &name, Primitive *prim,
				 Material *mat = nullptr);

	void setMaterial(Material *material);
	void addTexture(Texture *texture);

	glm::vec3 PointColor(const glm::vec2 &uv) const;

	Material *m_material;
	std::vector<Texture *> m_textures;

	Primitive *m_primitive;
};

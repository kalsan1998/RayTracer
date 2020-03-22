#pragma once

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
	void setTexture(Texture *texture);

	Material *m_material;
	Texture *m_texture;

	Primitive *m_primitive;
};

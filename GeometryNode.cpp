// Winter 2020

#include "GeometryNode.hpp"

#include "Material.hpp"
#include "Texture.hpp"
//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string &name, Primitive *prim, Material *mat)
	: SceneNode(name), m_material(mat), m_primitive(prim), m_textures()
{
	if (!m_material)
	{
		m_material = new Material();
	}
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial(Material *mat)
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

glm::vec3 GeometryNode::PointColor(const glm::vec2 &uv) const
{
	glm::vec3 col = m_material->Diffuse();
	for (auto *text : m_textures)
	{
		col += text->GetColor(uv);
	}
	return col;
}

void GeometryNode::addTexture(Texture *text)
{
	m_textures.push_back(text);
}

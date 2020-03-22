#pragma once

#include "Image.hpp"
#include <glm/glm.hpp>

class Texture
{
public:
    Texture(const Image &image);
    glm::vec3 GetColor(const glm::vec2 &uv) const;

private:
    Image m_image;
};
#include "Texture.hpp"

Texture::Texture(const Image &image) : m_image(image) {}

glm::vec3 Texture::GetColor(const glm::vec2 &uv) const
{
    uint scaled_u = uv.x * m_image.width();
    uint scaled_v = uv.y * m_image.height();
    return {m_image(scaled_u, scaled_v, 0),
            m_image(scaled_u, scaled_v, 1),
            m_image(scaled_u, scaled_v, 2)};
}

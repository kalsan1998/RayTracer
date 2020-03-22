#pragma once

#include "Image.hpp"

class Texture
{
public:
    Texture(const Image &image) : m_image(image) {}

private:
    Image m_image;
};
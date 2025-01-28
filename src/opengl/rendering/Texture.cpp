#include "opengl/rendering/Texture.h"
#include "opengl/Macros.h"
#include "opengl/opengl_include.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture()
    : id(0), filename(""), localBuffer(nullptr), width(0), height(0), bpp(0)
{
}
Texture::Texture(const std::string& path)
    : id(0), filename(path), localBuffer(nullptr), width(0), height(0), bpp(0)
{
    this->Load(path);
}

Texture::~Texture()
{
    if (this->id != 0)
    {
        GLCall(glDeleteTextures(1, &this->id));
    }
}

void Texture::Load(const std::string& path)
{
    if (this->id != 0)
    {
        GLCall(glDeleteTextures(1, &this->id));
    }

    stbi_set_flip_vertically_on_load(true);

    this->localBuffer = stbi_load(path.c_str(), &this->width, &this->height, &this->bpp, 4);
    if (!this->localBuffer)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    GLCall(glGenTextures(1, &this->id));

    if (this->id == 0)
    {
        std::cerr << "glGenTextures failed for texture: " << path << std::endl;
        stbi_image_free(this->localBuffer);
        return;
    }

    GLCall(glBindTexture(GL_TEXTURE_2D, this->id));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->localBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));// Unbind the texture

    if (this->localBuffer)
    {
        stbi_image_free(this->localBuffer);
    }
}

void Texture::Bind(unsigned int slot) const
{
    if (this->id == 0)
    {
        std::cerr << "Attempted to bind an invalid texture ID." << std::endl;
        return;
    }

    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, this->id));
}

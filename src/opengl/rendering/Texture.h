#pragma once

#include "opengl/opengl_include.h"
#include "opengl/rendering/RendererId.h"
#include <stb_image.h>
#include <string>

class Texture
{
public:
    Texture();
    explicit Texture(const std::string& path);
    ~Texture();

    // Delete copy constructor and copy assignment operator
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Implement move constructor
    Texture(Texture&& other) noexcept
        : id(other.id), filename(std::move(other.filename)),
          localBuffer(other.localBuffer), width(other.width),
          height(other.height), bpp(other.bpp)
    {
        other.id = 0;
        other.localBuffer = nullptr;
    }

    // Implement move assignment operator
    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            // Release existing resources
            if (id != 0)
                glDeleteTextures(1, &id);
            if (localBuffer)
                stbi_image_free(localBuffer);

            // Move resources from 'other' to 'this'
            id = other.id;
            filename = std::move(other.filename);
            localBuffer = other.localBuffer;
            width = other.width;
            height = other.height;
            bpp = other.bpp;

            // Nullify 'other'
            other.id = 0;
            other.localBuffer = nullptr;
        }
        return *this;
    }

    void Load(const std::string& path);
    void Bind(unsigned int slot = 0) const;

    [[nodiscard]] bool IsLoaded() const { return this->id != 0; }

    [[nodiscard]] inline int GetWidth() const { return this->width; }
    [[nodiscard]] inline int GetHeight() const { return this->height; }

private:
    RendererId id;
    std::string filename;
    unsigned char* localBuffer;
    int width, height, bpp;
};

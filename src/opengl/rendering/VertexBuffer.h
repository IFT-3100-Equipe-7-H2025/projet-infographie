#pragma once

#include "opengl/rendering/Renderer.h"

class VertexBuffer
{
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;

private:
    RendererId id;
    unsigned int size;
    const void* data;
};

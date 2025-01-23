#pragma once

#include "opengl/rendering/Renderer.h"
#include "opengl/rendering/VertexBuffer.h"
#include "opengl/rendering/VertexBufferLayout.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;

private:
    RendererId id;
};

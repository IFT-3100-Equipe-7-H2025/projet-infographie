#pragma once

#include "opengl/rendering/Renderer.h"

class VertexArrayObject
{
public:
    VertexArrayObject();
    ~VertexArrayObject();

    void Bind() const;

private:
    RendererId id;
};

#pragma once

#include "RendererId.h"

class VertexArrayObject
{
public:
    VertexArrayObject();
    ~VertexArrayObject();

    void Bind() const;

private:
    RendererId id;
};

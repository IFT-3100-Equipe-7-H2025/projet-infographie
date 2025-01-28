#pragma once

#include "opengl/Macros.h"
#include "opengl/opengl_include.h"

#include "opengl/rendering/VertexArray.h"
#include "opengl/rendering/IndexBuffer.h"
#include "opengl/rendering/Shader.h"


class Renderer
{
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
};

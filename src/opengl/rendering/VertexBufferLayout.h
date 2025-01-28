#pragma once

#include "opengl/opengl_include.h"
#include <vector>


struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    [[nodiscard]] static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:
                return sizeof(GLfloat);
            case GL_UNSIGNED_INT:
                return sizeof(GLuint);
            case GL_UNSIGNED_BYTE:
                return sizeof(GLubyte);
            default:
                return 0;
        }
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : elements(std::vector<VertexBufferElement>()), stride(0) {}

    template<typename T>
    void Push(unsigned int count)
    {
    }


    [[nodiscard]] inline unsigned int GetStride() const { return this->stride; }
    [[nodiscard]] inline const std::vector<VertexBufferElement>& GetElements() const { return this->elements; }

private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;
};

template<>
inline void VertexBufferLayout::Push<float>(const unsigned int count)
{
    this->elements.push_back({GL_FLOAT, count, GL_FALSE});
    this->stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

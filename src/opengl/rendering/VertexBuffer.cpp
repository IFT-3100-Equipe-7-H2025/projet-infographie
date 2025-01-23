#include "VertexBuffer.h"
#include "opengl/Macros.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) : id(0), size(size), data(data)
{
    GLCall(glGenBuffers(1, &this->id));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}


VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &this->id));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

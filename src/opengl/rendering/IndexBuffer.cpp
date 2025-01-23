#include "IndexBuffer.h"
#include "opengl/Macros.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : id(0)
{
    GLCall(glGenBuffers(1, &this->id));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &this->id));
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
}

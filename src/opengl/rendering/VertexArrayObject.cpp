#include "VertexArrayObject.h"
#include "opengl/Macros.h"

VertexArrayObject::VertexArrayObject() : id(0)
{
    GLCall(glGenVertexArrays(1, &this->id));
}

VertexArrayObject::~VertexArrayObject()
{
    GLCall(glDeleteVertexArrays(1, &this->id));
}

void VertexArrayObject::Bind() const
{
    GLCall(glBindVertexArray(this->id));
}

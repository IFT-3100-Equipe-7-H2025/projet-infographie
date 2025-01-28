#include "VertexArray.h"
#include "opengl/Macros.h"

VertexArray::VertexArray() : id(0)
{
    GLCall(glGenVertexArrays(1, &this->id));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &this->id));
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(this->id));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const
{
    this->Bind();
    vb.Bind();
    GLCall(glEnableVertexAttribArray(0));

    const std::vector<VertexBufferElement>& elements = layout.GetElements();

    unsigned int offset = 0;
    for (int i = 0; i < elements.size(); i++)
    {
        const VertexBufferElement& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(offset)));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

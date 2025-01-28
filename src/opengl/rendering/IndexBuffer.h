#pragma once

#include "RendererId.h"

class IndexBuffer
{
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind() const;

    [[nodiscard]] inline unsigned int GetCount() const { return this->count; }

private:
    RendererId id;
    unsigned int count;
};

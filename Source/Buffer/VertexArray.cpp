#include "VertexArray.hpp"

CVertexArray::CVertexArray()
{
    glGenVertexArrays(1, &Id);
}

void CVertexArray::LinkAttrib(CVertexBuffer *VertexBuffer, uint32_t Layout, uint32_t ComponentCount, uint32_t Type, ptrdiff_t Stride, void *Offset)
{
    VertexBuffer->Bind();
	glVertexAttribPointer(Layout, ComponentCount, Type, GL_FALSE, Stride, Offset);
	glEnableVertexAttribArray(Layout);
	VertexBuffer->Unbind();
}

void CVertexArray::Bind()
{
    glBindVertexArray(Id);
}

void CVertexArray::Unbind()
{
    glBindVertexArray(0);
}

void CVertexArray::Delete()
{
    glDeleteVertexArrays(1, &Id);
}
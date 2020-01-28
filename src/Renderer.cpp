#include "Renderer.h"

#include <iostream>



void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int type) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glDrawElements(type, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

}

#pragma once
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../VertexBuffer.h"
#include "../VertexArray.h"
#include "PolygonVertices.h"

struct Polygon {
	float r, g, b;
	float* vertices, *indices;
	IndexBuffer ib;
	Shader shader;
	VertexBuffer vb;
	VertexArray va;

	Polygon();

	void setRGB(float _r, float _g, float _b);
	void setBuffers();

	void generatePoints(const float* _v);
	
	
	
};
Polygon::Polygon() {
	va.Create();
	ib.
}

void Polygon::setBuffers() {

}
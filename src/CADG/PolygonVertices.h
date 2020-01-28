#pragma once

#include "../IndexBuffer.h"
//#include "../Shader.h"
#include "../VertexBuffer.h"
#include "../VertexArray.h"

#include <vector>

class PolygonVertices {
public:
	PolygonVertices();
	~PolygonVertices();

	void addVertex(float _x, float _y);
	void setRGB(float _r, float _g, float _b);
	//void removeVertex(float _x, float _y);

	bool isIn(float _x, float _y, int w, int h);
	int getClosestIn(float _x, float _y, int w);
	void removePoint(float _x, float _y, int w);
	void movePoint(float _x, float _y, int point_pos);
	void removeAll();

	int getUsedSize();
	const float* getArray();

	const std::vector<unsigned int>& getIndices();

private:
	int used_size;
	int size;
	float* vertices;
	std::vector<unsigned int> indices;

	void expand();
};
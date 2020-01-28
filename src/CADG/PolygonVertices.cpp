#include "PolygonVertices.h"
#include <cmath>
#include <iostream>

PolygonVertices::PolygonVertices() {
	size = 10;
	used_size = 0;
	vertices = new float[size];
}
PolygonVertices::~PolygonVertices() {
	delete[] vertices;
}

void PolygonVertices::addVertex(float _x, float _y) {
	if (used_size >= size) {
		expand();
	}

	vertices[used_size] = _x;
	vertices[used_size +1] = _y;
	used_size += 2;
	indices.push_back(indices.size());
}
//void PolygonVertices::removeVertex(float _x, float _y);

void PolygonVertices::expand() {
	size += 10;

	float *newArr = new float[size];

	for (int i = 0; i < used_size; i++) {
		newArr[i] = vertices[i];
	}

	delete[] vertices;
	vertices = newArr;
}

bool PolygonVertices::isIn(float _x, float _y, int w, int h) {
	_x = _x - w / 2;
	_y = h / 2 - _y;

	for (int i = 0; i < used_size; i += 2) {
		float r = sqrt((pow(_x - vertices[i], 2) + pow(_y - vertices[i+1], 2)));
		if (r <= 5.0f) {
			std::cout << "The cursor is in point (" << vertices[i] << ", " << vertices[i + 1] << ")" << std::endl;
		}
		
	}
	
	return false;
}

int PolygonVertices::getClosestIn(float _x, float _y, int w) {
	int currClosest = -1;
	float	currMinR = 100.0f;

	for (int i = 0; i < used_size; i+=2) {
		float r = sqrt(pow(vertices[i] - _x, 2) + pow(vertices[i + 1] - _y, 2));

		if (r <= 0.00904379971) {
			if (r <= currMinR) {
				currClosest = i;
				currMinR = r;
			}
		}
	}

	return currClosest;
}

void PolygonVertices::removePoint(float _x, float _y, int w) {
	int point = getClosestIn(_x, _y, w);

	if (point != -1) {
		for (int i = point; i < used_size - 2; i += 2) {
			vertices[i] = vertices[i + 2];
			vertices[i + 1] = vertices[i + 3];
		}

		used_size -= 2;
		indices.pop_back();
	}
}

void PolygonVertices::removeAll() {
	delete[] vertices;
	used_size = 0;
	size = 10;
	indices.clear();

	vertices = new float[size];
}

void PolygonVertices::movePoint(float _x, float _y, int point_pos) {
	if (point_pos != -1) {
		vertices[point_pos] = _x;
		vertices[point_pos + 1] = _y;
	}

}

const std::vector<unsigned int>& PolygonVertices::getIndices() {
	return indices;
}

int PolygonVertices::getUsedSize() {
	return used_size;
}

const float* PolygonVertices::getArray() {
	return vertices;
}


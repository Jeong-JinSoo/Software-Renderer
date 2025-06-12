#pragma once
#include "Transform.h"

struct Vertex
{
public:
	Vertex()
		:vPoint(0,0,0),vNormal(0,0,0),u(1.0f),v(1.0f), color(0xFF808080) {}
	Vertex(Point point, Point normal)
		:vPoint(point),vNormal(normal), u(1.0f), v(1.0f), color(0xFF808080) {}
	~Vertex() {}
public:
	// ������ ��ǥ
	Point vPoint;
	// normal ��ǥ
	Point vNormal;
	// �ؽ�Ʈ ��ǥ
	float u, v;
	// ���ؽ� ��;
	uint32_t color;
};
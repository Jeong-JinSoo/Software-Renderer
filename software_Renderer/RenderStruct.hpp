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
	// 포지션 좌표
	Point vPoint;
	// normal 좌표
	Point vNormal;
	// 텍스트 좌표
	float u, v;
	// 버텍스 색;
	uint32_t color;
};
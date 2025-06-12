#pragma once
#include <algorithm>
#include <cmath>
#include <array>

struct Triangle
{
	struct Vertex2D
	{
		// 픽셀 좌표
		float x;
		float y;

		float nx;
		float ny;

		// NDC -> 뷰포트 뒤 depth
		float z;
		float nz;
		
		// 1.0f / clipW(perspective)보정용
		float wInv;

		// 텍스처좌표
		float u, v;
	} v[3];

	struct AABB
	{
		int minX, maxX;
		int minY, maxY;
	};

	// 삼각형 AABB계산. 
	// 1. 삼각형의 세 정점(x,y)중 최솟값(minX,minY)과 최댓값(maxX, maxY)을 구하고
	// 2. 픽셀 단위 인덱스로 바꾼 뒤(내림/올림)
	// 3. 화면 범위로 한정(clamp) -> 이 박스 안에서만 "이 삼각형이 픽셀을 채울지(엣지 검사)를 반복한다.
	static AABB ComputeBoundingBox(const Triangle& tri, int screenWidth, int screenHeight);
};


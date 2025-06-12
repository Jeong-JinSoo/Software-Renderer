#include "Triangle.h"

Triangle::AABB Triangle::ComputeBoundingBox(const Triangle& tri, int screenWidth, int screenHeight)
{
	float fx0 = std::min({ tri.v[0].x, tri.v[1].x, tri.v[2].x, });
	float fx1 = std::max({ tri.v[0].x, tri.v[1].x, tri.v[2].x, });
	float fy0 = std::min({ tri.v[0].y, tri.v[1].y, tri.v[2].y, });
	float fy1 = std::max({ tri.v[0].y, tri.v[1].y, tri.v[2].y, });

	// std::floor(x); = 실수 x보다 같거나 작은 가장 큰 정수를 반환
	// std::ceil(x); = 실수 x보다 같거나 큰 가장 작은 정수를 반환
	// 이 두 함수를 통해 삼각형이 스크린에서 차지하는 모든 픽셀 범위를 정확하게 정수 인덱스 범위로 변환가능
	int minX = static_cast<int>(std::floor(fx0));
	int maxX = static_cast<int>(std::ceil(fx1));
	int minY = static_cast<int>(std::floor(fy0));
	int maxY = static_cast<int>(std::ceil(fy1));

	minX = std::clamp(minX, 0, screenWidth - 1);
	maxX = std::clamp(maxX, 0, screenWidth - 1);
	minY = std::clamp(minY, 0, screenHeight - 1);
	maxY = std::clamp(maxY, 0, screenHeight - 1);

	return AABB(minX,maxX,minY,maxY);
}

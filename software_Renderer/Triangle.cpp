#include "Triangle.h"

Triangle::AABB Triangle::ComputeBoundingBox(const Triangle& tri, int screenWidth, int screenHeight)
{
	float fx0 = std::min({ tri.v[0].x, tri.v[1].x, tri.v[2].x, });
	float fx1 = std::max({ tri.v[0].x, tri.v[1].x, tri.v[2].x, });
	float fy0 = std::min({ tri.v[0].y, tri.v[1].y, tri.v[2].y, });
	float fy1 = std::max({ tri.v[0].y, tri.v[1].y, tri.v[2].y, });

	// std::floor(x); = �Ǽ� x���� ���ų� ���� ���� ū ������ ��ȯ
	// std::ceil(x); = �Ǽ� x���� ���ų� ū ���� ���� ������ ��ȯ
	// �� �� �Լ��� ���� �ﰢ���� ��ũ������ �����ϴ� ��� �ȼ� ������ ��Ȯ�ϰ� ���� �ε��� ������ ��ȯ����
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

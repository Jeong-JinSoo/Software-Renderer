#pragma once
#include <algorithm>
#include <cmath>
#include <array>

struct Triangle
{
	struct Vertex2D
	{
		// �ȼ� ��ǥ
		float x;
		float y;

		float nx;
		float ny;

		// NDC -> ����Ʈ �� depth
		float z;
		float nz;
		
		// 1.0f / clipW(perspective)������
		float wInv;

		// �ؽ�ó��ǥ
		float u, v;
	} v[3];

	struct AABB
	{
		int minX, maxX;
		int minY, maxY;
	};

	// �ﰢ�� AABB���. 
	// 1. �ﰢ���� �� ����(x,y)�� �ּڰ�(minX,minY)�� �ִ�(maxX, maxY)�� ���ϰ�
	// 2. �ȼ� ���� �ε����� �ٲ� ��(����/�ø�)
	// 3. ȭ�� ������ ����(clamp) -> �� �ڽ� �ȿ����� "�� �ﰢ���� �ȼ��� ä����(���� �˻�)�� �ݺ��Ѵ�.
	static AABB ComputeBoundingBox(const Triangle& tri, int screenWidth, int screenHeight);
};


#pragma once
#include <vector>
#include <cstdint>
#include "Triangle.h"
#include "Matrix.hpp"
#include "RenderObject.h"

class Texture
{
public:
	// u, v in [0,1], �ݺ�-Ŭ������ ������ ����
	uint32_t Sample(float u, float v)const;
};

struct Rasterizer
{
public:
	static void RasterizerTriangle
	(
		const RenderObject& obj,				// ������ ������Ʈ
		std::vector<uint32_t>&	frameBuffer,	// �ȼ� ���� ���� (size = w*h)
		std::vector<float>&		depthBuffer,	// z-buffer (size = w*h)
		int						screenWidth,	// ȭ�� �ʺ�
		int						screenHeight	// ȭ�� ����
		);
		//const Texture&				texture		// ����� �ؽ�ó


	static void RasterizerLine(
		const RenderObject&			obj,			// �������� ������Ʈ
		int							screenWidth,	// ȭ�� �ʺ�
		int							screenHeight,	// ȭ�� ����
		std::vector<uint32_t>&		frameBuffer,	// ������ ����
		std::vector<float>&			depthBuffer	// ���� ����
	);

private:
	// �� ���� 2D A,B�� �ȼ� (Px, Py)�� ���� signed area * 2 ��ȯ
	static float EdgeFunction(const Triangle::Vertex2D& A, const Triangle::Vertex2D& B, float Px, float Py);
};


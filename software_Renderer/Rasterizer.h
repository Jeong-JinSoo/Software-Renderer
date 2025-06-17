#pragma once
#include <vector>
#include <cstdint>
#include "Triangle.h"
#include "Matrix.hpp"
#include "RenderObject.h"

class Texture
{
public:
	// u, v in [0,1], 반복-클램핑은 구현에 따라
	uint32_t Sample(float u, float v)const;
};

struct Rasterizer
{
public:
	static void RasterizerTriangle
	(
		const RenderObject& obj,				// 렌더할 오브젝트
		std::vector<uint32_t>&	frameBuffer,	// 픽셀 색상 버퍼 (size = w*h)
		std::vector<float>&		depthBuffer,	// z-buffer (size = w*h)
		int						screenWidth,	// 화면 너비
		int						screenHeight	// 화면 높이
		);
		//const Texture&				texture		// 사용할 텍스처


	static void RasterizerLine(
		const RenderObject&			obj,			// 렌더링할 오브젝트
		int							screenWidth,	// 화면 너비
		int							screenHeight,	// 화면 높이
		std::vector<uint32_t>&		frameBuffer,	// 프레임 버퍼
		std::vector<float>&			depthBuffer	// 뎁스 버퍼
	);

private:
	// 두 정점 2D A,B와 픽셀 (Px, Py)에 대한 signed area * 2 반환
	static float EdgeFunction(const Triangle::Vertex2D& A, const Triangle::Vertex2D& B, float Px, float Py);
};

